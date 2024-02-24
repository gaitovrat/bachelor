/*
 * LineTracer.h
 *
 *  Created on: Apr 10, 2023
 *      Author: Ratmir
 */

#ifndef LINETRACER_H_
#define LINETRACER_H_

#include <vector>
#include <utility>
#include <list>
#include <stddef.h>

#include "Region.h"
#include "Image.h"

class LineTracer {
public:
	LineTracer(uint32_t listSize);

	void AddImage(const Image &image, bool forceSearchRegions = false);

	unsigned int Left();

	unsigned int Right();

	Region LastRawRegion();

	bool HasLeft();

	bool HasRight();

	bool UnchangedLeft() const;
	bool UnchangedRight() const;

	Region Distances(const Image &image, bool hasPrevDistance,
			bool forceSearchRegions = false);

	bool FindByPreviousIndex(const Image &image, Region &foundRegion);

	std::vector<Region> Regions(const Image &image, uint8_t leftIndex = 0,
			uint8_t rightIndex = Image::LINE_LENGTH - 1,
			bool saveToClass = true);

	void Reset();

	size_t ListSize();

	std::pair<uint8_t, uint8_t>& DistancesPair();

private:
	std::vector<Region> m_currentRegions;
	uint32_t m_listSize;
	bool m_computedRegion;
	bool m_unchangedLeft;
	bool m_unchangedRight;
	uint8_t m_blackRegionsCount;
	uint8_t m_whiteRegionsCount;
	bool m_hasLeft;
	bool m_hasRight;
	std::pair<uint8_t, uint8_t> m_currentAverage;
	std::pair<uint8_t, uint8_t> m_currentMedian;
	std::list<Region> m_imageRegionList;
};
#endif /* LINETRACER_H_ */
