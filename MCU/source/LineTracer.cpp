#include <LineTracer.h>

#include <algorithm>

#include "fsl_debug_console.h"

#include "Utils.h"
#include "Shared/Utils.h"

#define TOLERANCE 3

#define REGION_STANDARD_SIZE 85
#define REGION_SIZE_COEFICIENT 0.3
#define REGION_COMPUTED_SIZE (REGION_STANDARD_SIZE*REGION_SIZE_COEFICIENT)
#define REGION_DISTANCE 30
#define PREV_LINE_SEARCH_REGION 5
#define MAX_REGIONS_COUNT 25

using namespace MCU;

LineTracer::LineTracer(const uint32_t listSize) :
		m_listSize { listSize }, m_blackRegionsCount { 0 }, m_whiteRegionsCount { 0 }, m_hasLeft {
				false }, m_hasRight { false } {
}

void LineTracer::AddImage(const Shared::Image &image, bool forceSearchRegions) {
	bool hasPrevLine = true;
	if (m_imageRegionList.size() > m_listSize) {
		m_imageRegionList.pop_front();
	}

	if (m_imageRegionList.empty())
		hasPrevLine = false;

	m_imageRegionList.emplace_back(
			Distances(image, hasPrevLine, forceSearchRegions));
}

Shared::Region LineTracer::Distances(const Shared::Image &image, bool hasPrevDistance,
		bool forceSearchRegions) {
	m_currentRegions.clear();
	Shared::Region biggestWhiteRegion;
	m_computedRegion = false;
	uint8_t right, left;

	if (image.IsLowDiversity()) {
		m_whiteRegionsCount = 1;
		m_blackRegionsCount = 0;

		biggestWhiteRegion.Color = Shared::Image::COLOR_WHITE;
		biggestWhiteRegion.Left = 0;
		biggestWhiteRegion.Right = Shared::Image::LINE_LENGTH;

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
		searchLeftIdx = biggestWhiteRegion.Left + 1;
		searchRightIdx = biggestWhiteRegion.Right - 1;
	} else {
		searchLeftIdx = Shared::Region::MIN_LEFT;
		searchRightIdx = Shared::Region::MAX_RIGHT;
	}

	Regions(image, searchLeftIdx, searchRightIdx);
	if (regionByPreviousIndexFound) {
		m_currentRegions.at(m_currentRegions.size() - 1).Right =
				biggestWhiteRegion.Right;
		m_currentRegions.at(0).Left = biggestWhiteRegion.Left;
	} else {
		m_currentRegions.at(m_currentRegions.size() - 1).Right =
		Shared::Image::LINE_LENGTH - 1;
		m_currentRegions.at(0).Left = 0;

	}
	if (!regionByPreviousIndexFound) {
		biggestWhiteRegion = { Shared::Region::MIN_LEFT, Shared::Region::MIN_LEFT, Shared::Image::COLOR_WHITE };
	}

	m_whiteRegionsCount = 0;
	m_blackRegionsCount = 0;

	for (Shared::Region &r : m_currentRegions) {
		if (r.IsWhite()) {
			m_whiteRegionsCount++;
			if ((r.Size() > biggestWhiteRegion.Size())) {
				biggestWhiteRegion = r;
			}
		} else {
			m_blackRegionsCount++;
		}
	}

	bool hasLeftLine = false;
	bool hasRightLine = false;
	for (Shared::Region &r : m_currentRegions) {
		if (r.IsBlack()) {
			if (r.Center() < biggestWhiteRegion.Left) {
				hasLeftLine = true;
			} else if (r.Center() > biggestWhiteRegion.Right) {
				hasRightLine = true;
			}
		}

	}

	if (!(hasLeftLine && hasRightLine)) {
		m_computedRegion = true;

		if (biggestWhiteRegion.Center() > Shared::Image::LINE_LENGTH / 2) {
			biggestWhiteRegion.Left = biggestWhiteRegion.Right - REGION_DISTANCE
					- REGION_COMPUTED_SIZE;
			biggestWhiteRegion.Right = biggestWhiteRegion.Right;
		} else {
			biggestWhiteRegion.Left = biggestWhiteRegion.Left;
			biggestWhiteRegion.Right = biggestWhiteRegion.Left + REGION_DISTANCE
					+ REGION_COMPUTED_SIZE;

		}
	}

	left = static_cast<uint8_t>(biggestWhiteRegion.Left);
	right = static_cast<uint8_t>(biggestWhiteRegion.Right);

	m_hasLeft = hasLeftLine;
	m_hasRight = hasRightLine;
	return {left, right, Shared::Image::COLOR_WHITE};
}

unsigned int LineTracer::Right() {
	std::vector<unsigned int> rightLines;
	rightLines.reserve(m_imageRegionList.size());

	unsigned int sum = 0;
	for (auto idx : m_imageRegionList) {
		rightLines.emplace_back(idx.Right);
		sum += idx.Right;
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
		leftLines.emplace_back(idx.Left);
		sum += idx.Left;
	}

	std::sort(leftLines.begin(), leftLines.end());

	unsigned int current = leftLines.at(leftLines.size() - 1);

	unsigned int average = sum / leftLines.size();

	m_unchangedLeft = ((current >= average - TOLERANCE)
			&& (current <= average + TOLERANCE));

	return average;
}

bool LineTracer::FindByPreviousIndex(const Shared::Image &image, Shared::Region &r) {
	uint8_t newLeftIndex = 0;
	uint8_t newRightIndex = 0;
	uint8_t status = 0b00000000;

#define foundLeftBitIndex   5
#define foundRightBitIndex  4

#define isFoundLeft()       BitRead(status, foundLeftBitIndex)
#define setFoundLeft(val)   BitWrite(status, foundLeftBitIndex ,val)

#define isFoundRight()      BitRead(status, foundRightBitIndex)
#define setFoundRight(val)  BitWrite(status, foundRightBitIndex ,val)

#define outOfBounds(index)  (index <= Shared::Image::BLACK_COUNT) || (index >= Shared::Image::LINE_LENGTH - Shared::Image::BLACK_COUNT - 1)

	Shared::Region previousRegion = m_imageRegionList.back();

	uint8_t prevLeftIndex = previousRegion.Left;
	uint8_t prevRightIndex = previousRegion.Right;
	uint8_t counter = 0;
	int8_t i = 0;
	while (std::abs(i) < static_cast<int32_t>(PREV_LINE_SEARCH_REGION)) {
		counter++;
		if (counter - 1 > static_cast<int32_t>(PREV_LINE_SEARCH_REGION * 2)) {
			break;
		}

		if (isFoundLeft() && isFoundRight()) {
			r.Left = newLeftIndex;
			r.Right = newRightIndex;
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

			if (leftLeftColor == Shared::Image::COLOR_BLACK && rightLeftColor == Shared::Image::COLOR_WHITE) {
				PRINTF("Found left index: %d\r\n", newLeftIndex);
				setFoundLeft(true);
			}
		}

		if (!isFoundRight()) {
			uint8_t leftRightColor = image.AtThresh(
					static_cast<const uint8_t>(newRightIndex - 1));

			uint8_t rightRightColor = image.AtThresh(newRightIndex);

			if (rightRightColor == Shared::Image::COLOR_BLACK
					&& leftRightColor == Shared::Image::COLOR_WHITE) {
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

std::vector<Shared::Region> LineTracer::Regions(const Shared::Image &image,
		uint8_t searchLeftIdx, uint8_t searchRightIdx, bool saveToClass) {
	PRINTF("Search indexes by region\r\n");
	std::vector<Shared::Region> foundRegions;

	uint8_t currentColor = static_cast<uint8_t>(image.AtThresh(searchLeftIdx));
	foundRegions.emplace_back(Shared::Region( { searchLeftIdx, searchLeftIdx,
			currentColor }));

	for (uint8_t i = searchLeftIdx; i <= searchRightIdx; i++) {
		/*NXP_TRACEP("idx: %03d\tpix: %03d"
		 NL, i, image.atThresh(i, ImgType::Thresholded));*/
		if (currentColor != image.AtThresh(i)) {
			if (foundRegions.size() > MAX_REGIONS_COUNT) {
				PRINTF("Found high region count. End searching.\r\n");
				break;
			}
			foundRegions.at(foundRegions.size() - 1).Right = i;
			foundRegions.emplace_back(Shared::Region( { i, i, image.AtThresh(i) }));
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
		leftDistances.emplace_back(idx.Left);
		rightDistances.emplace_back(idx.Right);

		sums.first += idx.Left;
		sums.second += idx.Right;
	}

	m_currentAverage.first = sums.first / m_imageRegionList.size();
	m_currentAverage.second = sums.second / m_imageRegionList.size();

	m_currentMedian.first = Shared::Utils::Median<uint8_t>(leftDistances);
	m_currentMedian.second = Shared::Utils::Median<uint8_t>(rightDistances);

	m_unchangedLeft = ((m_imageRegionList.back().Left
			>= m_currentAverage.first - TOLERANCE)
			&& (m_imageRegionList.back().Left
					<= m_currentAverage.first + TOLERANCE));
	m_unchangedRight = ((m_imageRegionList.back().Right
			>= m_currentAverage.second - TOLERANCE)
			&& (m_imageRegionList.back().Right
					<= m_currentAverage.second + TOLERANCE));

	currentDistances.first = m_imageRegionList.back().Left;
	currentDistances.second = m_imageRegionList.back().Right;

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

Shared::Region LineTracer::LastRawRegion() {
	return m_imageRegionList.back();
}

bool LineTracer::UnchangedLeft() const {
	return m_unchangedLeft;
}
bool LineTracer::UnchangedRight() const {
	return m_unchangedRight;
}
