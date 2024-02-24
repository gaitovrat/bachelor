#ifndef _IMAGE_H
#define _IMAGE_H

#include <stdint.h>

class Image {
public:
	enum Type {
		Raw,
		Normalized,
		Thresholded
	};

	static constexpr uint8_t LINE_LENGTH = 128u;
	static constexpr uint8_t COLOR_BLACK = 0x00;
	static constexpr uint8_t COLOR_WHITE = 0xff;
	static constexpr uint16_t COLOR_WHITE_ORIGINAL = 0xfff;
	static constexpr uint8_t LOW_DIVERSITY = 100;
	static constexpr uint8_t BLACK_COUNT = 10;

	Image();

	Image(uint16_t (&rawImage)[LINE_LENGTH]);

	uint16_t At(uint8_t index, Type type) const;

	uint8_t AtThresh(uint8_t index) const;

	uint16_t Min() const;

	uint16_t Max() const;

	uint16_t ThreshValue() const;

	int16_t Diversity() const;

	bool IsLowDiversity() const;

private:
	void ComputeMinMax(const uint16_t (&img)[LINE_LENGTH]);

	void Cut(uint16_t (&img)[LINE_LENGTH]);

	void Normalize(const uint16_t (&srcImg)[LINE_LENGTH],
			uint16_t (&dstImg)[LINE_LENGTH]);

	uint16_t AverageThreshold(
			const uint16_t (&srcImg)[LINE_LENGTH]);

	void Threshold(const uint16_t (&srcImg)[LINE_LENGTH],
			uint16_t (&dstImg)[LINE_LENGTH]);

	void SlowMedianBlur(const uint16_t (&srcImg)[LINE_LENGTH],
			uint16_t (&dstImg)[LINE_LENGTH], int pixels);

	void SlowMedianBlur(uint16_t (&srcImg)[LINE_LENGTH],
			int pixels);

	void FastMedianBlur(const uint16_t (&srcImg)[LINE_LENGTH],
			uint16_t (&dstImg)[LINE_LENGTH], int pixels);

	void FastMedianBlur(uint16_t (&srcImg)[LINE_LENGTH],
			int pixels);

private:
	uint16_t m_thresholdedImage[LINE_LENGTH];
	uint16_t m_max;
	uint16_t m_min;
	uint16_t m_threshValue;
	int16_t m_diversity;
};
#endif
