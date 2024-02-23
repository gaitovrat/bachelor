#ifndef _IMAGE_H
#define _IMAGE_H

#include <cstdint>

#include "Defines.h"
#include "ImageType.h"

class Image {
public:
	Image();

	Image(uint16_t (&rawImage)[TFC_CAMERA_LINE_LENGTH]);

	uint16_t At(uint8_t index, ImgType type) const;

	uint8_t AtThresh(uint8_t index) const;

	uint16_t Min() const;

	uint16_t Max() const;

	uint16_t ThreshValue() const;

	int16_t Diversity() const;

	bool IsLowDiversity() const;

protected:
	void ComputeMinMax(const uint16_t (&img)[TFC_CAMERA_LINE_LENGTH]);

	void Cut(uint16_t (&img)[TFC_CAMERA_LINE_LENGTH]);

	void Normalize(const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
			uint16_t (&dstImg)[TFC_CAMERA_LINE_LENGTH]);

	uint16_t AverageThreshold(
			const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH]);

	void Threshold(const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
			uint16_t (&dstImg)[TFC_CAMERA_LINE_LENGTH]);

	void SlowMedianBlur(const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
			uint16_t (&dstImg)[TFC_CAMERA_LINE_LENGTH], int pixels);

	void SlowMedianBlur(uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
			int pixels);

	void FastMedianBlur(const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
			uint16_t (&dstImg)[TFC_CAMERA_LINE_LENGTH], int pixels);

	void FastMedianBlur(uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
			int pixels);

protected:
	uint16_t m_thresholdedImage[TFC_CAMERA_LINE_LENGTH];
	uint16_t m_max;
	uint16_t m_min;
	uint16_t m_threshValue;
	int16_t m_diversity;
};
#endif
