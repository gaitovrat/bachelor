#include <LineTracer.h>

#include <algorithm>

#include "fsl_debug_console.h"

#include "Defines.h"
#include "CarMath.h"

#define TOLERANCE 3

#define REGION_STANDARD_SIZE 85
#define REGION_SIZE_COEFICIENT 0.3
#define REGION_COMPUTED_SIZE (REGION_STANDARD_SIZE*REGION_SIZE_COEFICIENT)
#define REGION_DISTANCE 30

LineTracer::LineTracer(const uint32_t listSize) :
		m_listSize { listSize }, m_blackRegionsCount { 0 }, m_whiteRegionsCount { 0 }, m_hasLeft {
				false }, m_hasRight { false } {
}

void LineTracer::AddImage(const Image &image, bool forceSearchRegions) {
	bool hasPrevLine = true;
	if (m_imageRegionList.size() > m_listSize) {
		m_imageRegionList.pop_front();
	}

	if (m_imageRegionList.empty())
		hasPrevLine = false;

	m_imageRegionList.emplace_back(
			Distances(image, hasPrevLine, forceSearchRegions));
}

Region LineTracer::Distances(const Image &image, bool hasPrevDistance,
		bool forceSearchRegions) {
	m_currentRegions.clear();
	Region biggestWhiteRegion;
	m_computedRegion = false;
	uint8_t right, left;

	if (image.IsLowDiversity()) {
		m_whiteRegionsCount = 1;
		m_blackRegionsCount = 0;

		biggestWhiteRegion.color = COLOR_WHITE;
		biggestWhiteRegion.left = 0;
		biggestWhiteRegion.right = TFC_CAMERA_LINE_LENGTH;

		m_currentRegions.emplace_back(biggestWhiteRegion);
		return biggestWhiteRegion;
	}

	bool regionByPreviousIndexFound = false;
	if (hasPrevDistance) {
		regionByPreviousIndexFound = FindByPreviousIndex(image,
				biggestWhiteRegion);
		if (regionByPreviousIndexFound) {
			m_currentRegions.emplace_back(biggestWhiteRegion);
			m_whiteRegionsCount = 1;
			m_blackRegionsCount = 2;
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
		searchLeftIdx = Region::minLeft;
		searchRightIdx = Region::maxRight;
	}

	Regions(image, searchLeftIdx, searchRightIdx);
	if (regionByPreviousIndexFound) {
		m_currentRegions.at(m_currentRegions.size() - 1).right =
				biggestWhiteRegion.right;
		m_currentRegions.at(0).left = biggestWhiteRegion.left;
	} else {
		m_currentRegions.at(m_currentRegions.size() - 1).right =
		TFC_CAMERA_LINE_LENGTH - 1;
		m_currentRegions.at(0).left = 0;

	}
	if (!regionByPreviousIndexFound) {
		biggestWhiteRegion = { Region::minLeft, Region::minLeft, COLOR_WHITE };
	}

	m_whiteRegionsCount = 0;
	m_blackRegionsCount = 0;

	for (Region &r : m_currentRegions) {
		if (r.isWhite()) {
			m_whiteRegionsCount++;
			if ((r.getSize() > biggestWhiteRegion.getSize())) {
				biggestWhiteRegion = r;
			}
		} else {
			m_blackRegionsCount++;
		}
	}

	bool hasLeftLine = false;
	bool hasRightLine = false;
	for (Region &r : m_currentRegions) {
		if (r.isBlack()) {
			if (r.getCenter() < biggestWhiteRegion.left) {
				hasLeftLine = true;
			} else if (r.getCenter() > biggestWhiteRegion.right) {
				hasRightLine = true;
			}
		}

	}

	if (!(hasLeftLine && hasRightLine)) {
		m_computedRegion = true;

		if (biggestWhiteRegion.getCenter() > TFC_CAMERA_LINE_LENGTH / 2) {
			biggestWhiteRegion.left = biggestWhiteRegion.right - REGION_DISTANCE
					- REGION_COMPUTED_SIZE;
			biggestWhiteRegion.right = biggestWhiteRegion.right;
		} else {
			biggestWhiteRegion.left = biggestWhiteRegion.left;
			biggestWhiteRegion.right = biggestWhiteRegion.left + REGION_DISTANCE
					+ REGION_COMPUTED_SIZE;

		}
	}

	PRINTF("Found region count: WHITE: %d BLACK: %d\r\n", m_whiteRegionsCount,
			m_blackRegionsCount);

	PRINTF(
			"Highest white region: Left index: %d, Right index: %d, Middle %d, Size %d\r\n",
			biggestWhiteRegion.left, biggestWhiteRegion.right,
			biggestWhiteRegion.getCenter(), biggestWhiteRegion.getSize());

	left = static_cast<uint8_t>(biggestWhiteRegion.left);
	right = static_cast<uint8_t>(biggestWhiteRegion.right);

	for (Region &region : m_currentRegions) {
		PRINTF(
				"Region: %s size: %03d, middle: %03d, left: %03d, right: %03d\r\n",
				region.color == COLOR_BLACK ? "BLACK" : "WHITE",
				region.getSize(), region.getCenter(), region.left,
				region.right);
	}
	m_hasLeft = hasLeftLine;
	m_hasRight = hasRightLine;
	return {left, right, COLOR_WHITE};
}

unsigned int LineTracer::Right() {
	std::vector<unsigned int> rightLines;
	rightLines.reserve(m_imageRegionList.size());

	unsigned int sum = 0;
	for (auto idx : m_imageRegionList) {
		rightLines.emplace_back(idx.right);
		sum += idx.right;
	}

	std::sort(rightLines.begin(), rightLines.end());

	unsigned int current = rightLines.at(rightLines.size() - 1);

	unsigned int average = sum / rightLines.size();

	m_unchangedRight = ((current >= average - TOLERANCE)
			&& (current <= average + TOLERANCE));

	return average;
}

unsigned int LineTracer::Left() {
	std::vector<unsigned int> leftLines;
	leftLines.reserve(m_imageRegionList.size());

	unsigned int sum = 0;
	for (auto idx : m_imageRegionList) {
		leftLines.emplace_back(idx.left);
		sum += idx.left;
	}

	std::sort(leftLines.begin(), leftLines.end());

	unsigned int current = leftLines.at(leftLines.size() - 1);

	unsigned int average = sum / leftLines.size();

	m_unchangedLeft = ((current >= average - TOLERANCE)
			&& (current <= average + TOLERANCE));

	return average;
}

bool LineTracer::FindByPreviousIndex(const Image &image, Region &r) {

	uint8_t newLeftIndex = 0;
	uint8_t newRightIndex = 0;
	uint8_t status = 0b00000000;

#define foundLeftBitIndex   5
#define foundRightBitIndex  4

#define isFoundLeft()       bitRead(status, foundLeftBitIndex)
#define setFoundLeft(val)   bitWrite(status, foundLeftBitIndex ,val)

#define isFoundRight()      bitRead(status, foundRightBitIndex)
#define setFoundRight(val)  bitWrite(status, foundRightBitIndex ,val)

#define outOfBounds(index)  (index <= BLACK_COUNT) || (index >= TFC_CAMERA_LINE_LENGTH - BLACK_COUNT - 1)

	Region previousRegion = m_imageRegionList.back();

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
			uint8_t leftLeftColor = image.AtThresh(newLeftIndex);
			uint8_t rightLeftColor = image.AtThresh(newLeftIndex + 1);

			if (leftLeftColor == COLOR_BLACK && rightLeftColor == COLOR_WHITE) {
				PRINTF("Found left index: %d\r\n", newLeftIndex);
				setFoundLeft(true);
			}
		}

		if (!isFoundRight()) {
			uint8_t leftRightColor = image.AtThresh(
					static_cast<const uint8_t>(newRightIndex - 1));

			uint8_t rightRightColor = image.AtThresh(newRightIndex);

			if (rightRightColor == COLOR_BLACK
					&& leftRightColor == COLOR_WHITE) {
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

void LineTracer::Reset() {
	this->m_imageRegionList.clear();
}

std::vector<Region> LineTracer::Regions(const Image &image,
		uint8_t searchLeftIdx, uint8_t searchRightIdx, bool saveToClass) {
	PRINTF("Search indexes by region\r\n");
	std::vector<Region> foundRegions;

	uint8_t currentColor = static_cast<uint8_t>(image.AtThresh(searchLeftIdx));
	foundRegions.emplace_back(Region( { searchLeftIdx, searchLeftIdx,
			currentColor }));

	for (uint8_t i = searchLeftIdx; i <= searchRightIdx; i++) {
		/*NXP_TRACEP("idx: %03d\tpix: %03d"
		 NL, i, image.atThresh(i, ImgType::Thresholded));*/
		if (currentColor != image.AtThresh(i)) {
			if (foundRegions.size() > MAX_REGIONS_COUNT) {
				PRINTF("Found high region count. End searching.\r\n");
				break;
			}
			foundRegions.at(foundRegions.size() - 1).right = i;
			foundRegions.emplace_back(Region( { i, i, image.AtThresh(i) }));
		}
		currentColor = static_cast<uint8_t>(image.AtThresh(i));
	}

	if (saveToClass) {
		m_currentRegions = foundRegions;
	}

	return foundRegions;
}

std::pair<uint8_t, uint8_t>& LineTracer::DistancesPair() {
	std::pair<uint8_t, uint8_t> currentDistances = { 0, 0 };
	std::pair<uint16_t, uint16_t> sums = { 0, 0 };
	std::vector<uint8_t> leftDistances;
	std::vector<uint8_t> rightDistances;

	leftDistances.reserve(m_imageRegionList.size());
	rightDistances.reserve(m_imageRegionList.size());

	for (auto idx : m_imageRegionList) {
		leftDistances.emplace_back(idx.left);
		rightDistances.emplace_back(idx.right);

		sums.first += idx.left;
		sums.second += idx.right;
	}

	m_currentAverage.first = sums.first / m_imageRegionList.size();
	m_currentAverage.second = sums.second / m_imageRegionList.size();

	m_currentMedian.first = median<uint8_t>(leftDistances);
	m_currentMedian.second = median<uint8_t>(rightDistances);

	m_unchangedLeft = ((m_imageRegionList.back().left
			>= m_currentAverage.first - TOLERANCE)
			&& (m_imageRegionList.back().left
					<= m_currentAverage.first + TOLERANCE));
	m_unchangedRight = ((m_imageRegionList.back().right
			>= m_currentAverage.second - TOLERANCE)
			&& (m_imageRegionList.back().right
					<= m_currentAverage.second + TOLERANCE));

	currentDistances.first = m_imageRegionList.back().left;
	currentDistances.second = m_imageRegionList.back().right;

	PRINTF("Current distances: first %d, second %d\r\n", currentDistances.first,
			currentDistances.second);

	return m_currentAverage;
}

size_t LineTracer::ListSize() {
	return m_imageRegionList.size();
}

bool LineTracer::HasLeft() {
	return m_hasLeft;
}

bool LineTracer::HasRight() {
	return m_hasRight;
}

Region LineTracer::LastRawRegion() {
	return m_imageRegionList.back();
}

bool LineTracer::UnchangedLeft() const {
	return m_unchangedLeft;
}
bool LineTracer::UnchangedRight() const {
	return m_unchangedRight;
}
