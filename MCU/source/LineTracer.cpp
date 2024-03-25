#include <LineTracer.h>

#include <algorithm>

#include "fsl_debug_console.h"

#include "Shared/Utils.h"
#include "Utils.h"

#define TOLERANCE 3

#define REGION_STANDARD_SIZE 85
#define REGION_SIZE_COEFICIENT 0.3
#define REGION_COMPUTED_SIZE (REGION_STANDARD_SIZE * REGION_SIZE_COEFICIENT)
#define REGION_DISTANCE 30
#define PREV_LINE_SEARCH_REGION 5
#define MAX_REGIONS_COUNT 25

using namespace MCU;

LineTracer::LineTracer(const uint32_t listSize)
    : listSize{listSize}, blackRegionsCount{0}, whiteRegionsCount{0},
      hasLeft{false}, hasRight{false} {}

void LineTracer::addImage(const Shared::Image &image, bool forceSearchRegions) {
    bool hasPrevLine = true;
    if (imageRegionList.size() > listSize) {
        imageRegionList.pop_front();
    }

    if (imageRegionList.empty())
        hasPrevLine = false;

    imageRegionList.emplace_back(
        distances(image, hasPrevLine, forceSearchRegions));
}

Shared::Region LineTracer::distances(const Shared::Image &image,
                                     bool hasPrevDistance,
                                     bool forceSearchRegions) {
    currentRegions.clear();
    Shared::Region biggestWhiteRegion;
    computedRegion = false;
    uint8_t right, left;

    if (image.isLowDiversity()) {
        whiteRegionsCount = 1;
        blackRegionsCount = 0;

        biggestWhiteRegion.color = Shared::Image::COLOR_WHITE;
        biggestWhiteRegion.left = 0;
        biggestWhiteRegion.right = Shared::Image::LINE_LENGTH;

        currentRegions.emplace_back(biggestWhiteRegion);
        return biggestWhiteRegion;
    }

    bool regionByPreviousIndexFound = false;
    if (hasPrevDistance) {
        regionByPreviousIndexFound =
            findByPreviousIndex(image, biggestWhiteRegion);
        if (regionByPreviousIndexFound) {
            currentRegions.emplace_back(biggestWhiteRegion);
            whiteRegionsCount = 1;
            blackRegionsCount = 2;
            if (!forceSearchRegions)
                return biggestWhiteRegion;
        }
    }

    uint8_t searchLeftIdx;
    uint8_t searchRightIdx;

    if (regionByPreviousIndexFound) {
        searchLeftIdx = biggestWhiteRegion.left + 1;
        searchRightIdx = biggestWhiteRegion.right - 1;
    } else {
        searchLeftIdx = Shared::Region::MIN_LEFT;
        searchRightIdx = Shared::Region::MAX_RIGHT;
    }

    regions(image, searchLeftIdx, searchRightIdx);
    if (regionByPreviousIndexFound) {
        currentRegions.at(currentRegions.size() - 1).right =
            biggestWhiteRegion.right;
        currentRegions.at(0).left = biggestWhiteRegion.left;
    } else {
        currentRegions.at(currentRegions.size() - 1).right =
            Shared::Image::LINE_LENGTH - 1;
        currentRegions.at(0).left = 0;
    }
    if (!regionByPreviousIndexFound) {
        biggestWhiteRegion = {Shared::Region::MIN_LEFT,
                              Shared::Region::MIN_LEFT,
                              Shared::Image::COLOR_WHITE};
    }

    whiteRegionsCount = 0;
    blackRegionsCount = 0;

    for (Shared::Region &r : currentRegions) {
        if (r.isWhite()) {
            whiteRegionsCount++;
            if ((r.size() > biggestWhiteRegion.size())) {
                biggestWhiteRegion = r;
            }
        } else {
            blackRegionsCount++;
        }
    }

    bool hasLeftLine = false;
    bool hasRightLine = false;
    for (Shared::Region &r : currentRegions) {
        if (r.isBlack()) {
            if (r.center() < biggestWhiteRegion.left) {
                hasLeftLine = true;
            } else if (r.center() > biggestWhiteRegion.right) {
                hasRightLine = true;
            }
        }
    }

    if (!(hasLeftLine && hasRightLine)) {
        computedRegion = true;

        if (biggestWhiteRegion.center() > Shared::Image::LINE_LENGTH / 2) {
            biggestWhiteRegion.left = biggestWhiteRegion.right -
                                      REGION_DISTANCE - REGION_COMPUTED_SIZE;
            biggestWhiteRegion.right = biggestWhiteRegion.right;
        } else {
            biggestWhiteRegion.left = biggestWhiteRegion.left;
            biggestWhiteRegion.right = biggestWhiteRegion.left +
                                       REGION_DISTANCE + REGION_COMPUTED_SIZE;
        }
    }

    left = static_cast<uint8_t>(biggestWhiteRegion.left);
    right = static_cast<uint8_t>(biggestWhiteRegion.right);

    hasLeft = hasLeftLine;
    hasRight = hasRightLine;
    return {left, right, Shared::Image::COLOR_WHITE};
}

unsigned int LineTracer::right() {
    std::vector<unsigned int> rightLines;
    rightLines.reserve(imageRegionList.size());

    unsigned int sum = 0;
    for (auto idx : imageRegionList) {
        rightLines.emplace_back(idx.right);
        sum += idx.right;
    }

    std::sort(rightLines.begin(), rightLines.end());

    unsigned int current = rightLines.at(rightLines.size() - 1);

    unsigned int average = sum / rightLines.size();

    unchangedRight =
        ((current >= average - TOLERANCE) && (current <= average + TOLERANCE));

    return average;
}

unsigned int LineTracer::left() {
    std::vector<unsigned int> leftLines;
    leftLines.reserve(imageRegionList.size());

    unsigned int sum = 0;
    for (auto idx : imageRegionList) {
        leftLines.emplace_back(idx.left);
        sum += idx.left;
    }

    std::sort(leftLines.begin(), leftLines.end());

    unsigned int current = leftLines.at(leftLines.size() - 1);

    unsigned int average = sum / leftLines.size();

    unchangedLeft =
        ((current >= average - TOLERANCE) && (current <= average + TOLERANCE));

    return average;
}

bool LineTracer::findByPreviousIndex(const Shared::Image &image,
                                     Shared::Region &r) {
    uint8_t newLeftIndex = 0;
    uint8_t newRightIndex = 0;
    uint8_t status = 0b00000000;

#define foundLeftBitIndex 5
#define foundRightBitIndex 4

#define isFoundLeft() bitRead(status, foundLeftBitIndex)
#define setFoundLeft(val) bitWrite(status, foundLeftBitIndex, val)

#define isFoundRight() bitRead(status, foundRightBitIndex)
#define setFoundRight(val) bitWrite(status, foundRightBitIndex, val)

#define outOfBounds(index)                                                     \
    (index <= Shared::Image::BLACK_COUNT) ||                                   \
        (index >= Shared::Image::LINE_LENGTH - Shared::Image::BLACK_COUNT - 1)

    Shared::Region previousRegion = imageRegionList.back();

    uint8_t prevLeftIndex = previousRegion.left;
    uint8_t prevRightIndex = previousRegion.right;
    uint8_t counter = 0;
    int8_t i = 0;
    while (std::abs(i) < static_cast<int32_t>(PREV_LINE_SEARCH_REGION)) {
        counter++;
        if (counter - 1 > static_cast<int32_t>(PREV_LINE_SEARCH_REGION * 2)) {
            break;
        }

        if (isFoundLeft() && isFoundRight()) {
            r.left = newLeftIndex;
            r.right = newRightIndex;
            break;
        }

        if (!isFoundLeft()) {
            newLeftIndex = static_cast<uint8_t>(prevLeftIndex + i);
        }

        if (!isFoundRight()) {
            newRightIndex = static_cast<uint8_t>(prevRightIndex + i);
        }

        if (outOfBounds(newLeftIndex) || outOfBounds(newRightIndex)) {
            continue;
        }

        if (!isFoundLeft()) {
            uint8_t leftLeftColor = image.atThresh(newLeftIndex);
            uint8_t rightLeftColor = image.atThresh(newLeftIndex + 1);

            if (leftLeftColor == Shared::Image::COLOR_BLACK &&
                rightLeftColor == Shared::Image::COLOR_WHITE) {
                PRINTF("Found left index: %d\r\n", newLeftIndex);
                setFoundLeft(true);
            }
        }

        if (!isFoundRight()) {
            uint8_t leftRightColor =
                image.atThresh(static_cast<const uint8_t>(newRightIndex - 1));

            uint8_t rightRightColor = image.atThresh(newRightIndex);

            if (rightRightColor == Shared::Image::COLOR_BLACK &&
                leftRightColor == Shared::Image::COLOR_WHITE) {
                PRINTF("Found right index: %d\r\n", newRightIndex);
                setFoundRight(true);
            }
        }

        i *= -1;
        if (i >= 0) {
            i++;
        }
    }

    return isFoundLeft() && isFoundRight();
}

void LineTracer::reset() { this->imageRegionList.clear(); }

std::vector<Shared::Region> LineTracer::regions(const Shared::Image &image,
                                                uint8_t searchLeftIdx,
                                                uint8_t searchRightIdx,
                                                bool saveToClass) {
    PRINTF("Search indexes by region\r\n");
    std::vector<Shared::Region> foundRegions;

    uint8_t currentColor = static_cast<uint8_t>(image.atThresh(searchLeftIdx));
    foundRegions.emplace_back(
        Shared::Region({searchLeftIdx, searchLeftIdx, currentColor}));

    for (uint8_t i = searchLeftIdx; i <= searchRightIdx; i++) {
        /*NXP_TRACEP("idx: %03d\tpix: %03d"
         NL, i, image.atThresh(i, ImgType::Thresholded));*/
        if (currentColor != image.atThresh(i)) {
            if (foundRegions.size() > MAX_REGIONS_COUNT) {
                PRINTF("Found high region count. End searching.\r\n");
                break;
            }
            foundRegions.at(foundRegions.size() - 1).right = i;
            foundRegions.emplace_back(
                Shared::Region({i, i, image.atThresh(i)}));
        }
        currentColor = static_cast<uint8_t>(image.atThresh(i));
    }

    if (saveToClass) {
        currentRegions = foundRegions;
    }

    return foundRegions;
}

std::pair<uint8_t, uint8_t> &LineTracer::distancesPair() {
    std::pair<uint8_t, uint8_t> currentDistances = {0, 0};
    std::pair<uint16_t, uint16_t> sums = {0, 0};
    std::vector<uint8_t> leftDistances;
    std::vector<uint8_t> rightDistances;

    leftDistances.reserve(imageRegionList.size());
    rightDistances.reserve(imageRegionList.size());

    for (auto idx : imageRegionList) {
        leftDistances.emplace_back(idx.left);
        rightDistances.emplace_back(idx.right);

        sums.first += idx.left;
        sums.second += idx.right;
    }

    currentAverage.first = sums.first / imageRegionList.size();
    currentAverage.second = sums.second / imageRegionList.size();

    currentMedian.first = Shared::Utils::median<uint8_t>(leftDistances);
    currentMedian.second = Shared::Utils::median<uint8_t>(rightDistances);

    unchangedLeft =
        ((imageRegionList.back().left >= currentAverage.first - TOLERANCE) &&
         (imageRegionList.back().left <= currentAverage.first + TOLERANCE));
    unchangedRight =
        ((imageRegionList.back().right >= currentAverage.second - TOLERANCE) &&
         (imageRegionList.back().right <= currentAverage.second + TOLERANCE));

    currentDistances.first = imageRegionList.back().left;
    currentDistances.second = imageRegionList.back().right;

    PRINTF("Current distances: first %d, second %d\r\n", currentDistances.first,
           currentDistances.second);

    return currentAverage;
}

size_t LineTracer::size() { return imageRegionList.size(); }

bool LineTracer::getHasLeft() { return hasLeft; }

bool LineTracer::getHasRight() { return hasRight; }

Shared::Region LineTracer::lastRawRegion() { return imageRegionList.back(); }

bool LineTracer::getUnchangedLeft() const { return unchangedLeft; }
bool LineTracer::getUnchangedRight() const { return unchangedRight; }
