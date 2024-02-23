#include <deque>
#include <cstring>

#include "Image.h"
#include "Defines.h"
#include "ImageType.h"
#include "CarMath.h"

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

Image::Image() :
		m_thresholdedImage { 0 }, m_max { COLOR_BLACK }, m_min { COLOR_WHITE } {

}

void Image::ComputeMinMax(const uint16_t (&img)[TFC_CAMERA_LINE_LENGTH]) {
	m_max = COLOR_BLACK;
	//min_ = COLOR_WHITE;
	m_min = COLOR_WHITE_ORIGINAL;
	for (uint64_t i = BLACK_COUNT; i < TFC_CAMERA_LINE_LENGTH - BLACK_COUNT;
			i++) {
		uint16_t pixel = img[i];
		if (pixel > m_max) {
			m_max = pixel;
			/*NXP_TRACEP("max_: %04d"
			 NL, max_);*/
		}

		if (pixel < m_min) {
			m_min = pixel;
			/*NXP_TRACEP("min_: %04d"
			 NL, min_);*/
		}

		if (m_max == COLOR_WHITE_ORIGINAL/*COLOR_WHITE*/&& m_min == COLOR_BLACK) {
			break;
		}

	}

	m_diversity = MAX(m_max, m_min) - MIN(m_min, m_max);
}

void Image::Cut(uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH]) {
	for (uint64_t col = 0; col < BLACK_COUNT; col++) {
		srcImg[col] = m_max;
	}

	for (uint64_t col = TFC_CAMERA_LINE_LENGTH - 1;
			col > TFC_CAMERA_LINE_LENGTH - 1 - BLACK_COUNT; col--) {
		srcImg[col] = m_max;
	}

}

void Image::Normalize(const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
		uint16_t (&dstImg)[TFC_CAMERA_LINE_LENGTH]) {
	for (int i = 0; i < TFC_CAMERA_LINE_LENGTH; i++) {
		float pixel = static_cast<float>(srcImg[i]);
		pixel -= m_min;
		pixel *= COLOR_WHITE;
		pixel /= (m_max - m_min);
		dstImg[i] = static_cast<uint16_t>(pixel);
	}
}

uint16_t Image::AverageThreshold(
		const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH]) {
	long sum = 0;
	for (uint64_t i = BLACK_COUNT; i < TFC_CAMERA_LINE_LENGTH - BLACK_COUNT;
			i++) {
		sum += srcImg[i];
	}
	auto avg = static_cast<uint16_t>(sum / TFC_CAMERA_LINE_LENGTH
			- (2 * BLACK_COUNT));
	return avg;
}

void Image::Threshold(const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
		uint16_t (&dstImg)[TFC_CAMERA_LINE_LENGTH]) {
	for (int i = 0; i < TFC_CAMERA_LINE_LENGTH; i++) {
		if (srcImg[i] < m_threshValue)
			dstImg[i] = COLOR_BLACK;
		else
			dstImg[i] = COLOR_WHITE;
	}
}

void Image::FastMedianBlur(uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
		const int pixels) {

	const int buffSize = pixels * 2 + 1;
	std::vector<uint16_t> blurBuffer;

	for (int i = pixels; i < TFC_CAMERA_LINE_LENGTH - pixels; i += buffSize) {
		for (int j = -pixels; j <= pixels; j++) {
			blurBuffer.emplace_back(srcImg[i + j]);
		}

		for (int j = -pixels; j <= pixels; j++) {
			srcImg[i + j] = median(blurBuffer);
		}
		blurBuffer.clear();
	}

}

void Image::FastMedianBlur(const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
		uint16_t (&dstImg)[TFC_CAMERA_LINE_LENGTH], const int pixels) {
	memcpy(dstImg, srcImg, TFC_CAMERA_LINE_LENGTH);

	const int buffSize = pixels * 2 + 1;
	std::vector<uint16_t> blurBuffer;

	for (int i = ((TFC_CAMERA_LINE_LENGTH / 2) % pixels) + pixels;
			i < TFC_CAMERA_LINE_LENGTH - pixels; i += buffSize) {
		for (int j = -pixels; j <= pixels; j++) {
			blurBuffer.emplace_back(srcImg[i + j]);
		}
		std::sort(blurBuffer.begin(), blurBuffer.end());
		for (int j = -pixels; j <= pixels; j++) {
			dstImg[i + j] = blurBuffer.at(pixels + 1);
		}
		blurBuffer.clear();
	}
}

void Image::SlowMedianBlur(const uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
		uint16_t (&dstImg)[TFC_CAMERA_LINE_LENGTH], const int pixels) {
	memcpy(dstImg, srcImg, TFC_CAMERA_LINE_LENGTH);
	std::vector<uint16_t> blurBuffer;

	for (int i = pixels; i < TFC_CAMERA_LINE_LENGTH - pixels; i++) {
		for (int j = -pixels; j <= pixels; j++) {
			blurBuffer.emplace_back(srcImg[i - j]);
		}
		std::sort(blurBuffer.begin(), blurBuffer.end());
		dstImg[i] = blurBuffer.at(pixels + 1);
		blurBuffer.clear();
	}
}

void Image::SlowMedianBlur(uint16_t (&srcImg)[TFC_CAMERA_LINE_LENGTH],
		int pixels) {
	std::deque<uint16_t> pixBuffer;

	for (int i = /*0*/1; i < TFC_CAMERA_LINE_LENGTH; i++) {
		if (static_cast<int>(pixBuffer.size()) < pixels) {
			pixBuffer.emplace_back(srcImg[i]);
			continue;
		}
		pixBuffer.emplace_back(srcImg[i]);

		for (int j = -pixels; j <= pixels; j++) {
			srcImg[i + j] = median<uint16_t>(
					std::vector<uint16_t>(pixBuffer.begin(), pixBuffer.end()));
		}
		//blurBuffer.clear();
		pixBuffer.pop_front();
	}
}

uint8_t Image::AtThresh(uint8_t index) const {
    return static_cast<uint8_t>(m_thresholdedImage[index]);
}

uint16_t Image::Min() const {
	return m_min;
}

uint16_t Image::Max() const {
	return m_max;
}

uint16_t Image::ThreshValue() const {
	return m_threshValue;
}

int16_t Image::Diversity() const {
	return m_diversity;
}

bool Image::IsLowDiversity() const {
	return m_diversity < LOW_DIVERSITY;
}

Image::Image(uint16_t (&rawImage)[TFC_CAMERA_LINE_LENGTH]) :
		Image() {
	ComputeMinMax(rawImage);
	if (m_diversity < LOW_DIVERSITY) {
		for (int i = 0; i < TFC_CAMERA_LINE_LENGTH; i++) {
			m_thresholdedImage[i] = COLOR_WHITE;
			rawImage[i] = COLOR_WHITE_ORIGINAL;
		}
	} else {
		Cut(rawImage);

		Normalize(rawImage, m_thresholdedImage);

		m_threshValue = AverageThreshold(m_thresholdedImage);
		Threshold(m_thresholdedImage, m_thresholdedImage);
	}

}

uint16_t Image::At(uint8_t index, ImgType type) const {
	if (index >= 128) {
		return 0;
	}
	switch (type) {
	case ImgType::Thresholded:
		return m_thresholdedImage[index];
	default:
		return 0;
	}
}
