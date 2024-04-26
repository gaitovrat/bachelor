#include "Shared/Image.h"

#include <cstring>
#include <deque>

#include "Shared/Utils.h"

using namespace Shared;

Image::Image()
    : thresholdedImage{0}, maxValue{COLOR_BLACK}, minValue{COLOR_WHITE} {}

void Image::computeMinMax(RefCImageLine img) {
    this->maxValue = COLOR_BLACK;
    this->minValue = COLOR_WHITE_ORIGINAL;

    for (uint64_t i = BLACK_COUNT; i < LINE_LENGTH - BLACK_COUNT; i++) {
        uint16_t pixel = img[i];
        if (pixel > this->maxValue) {
            this->maxValue = pixel;
        }

        if (pixel < this->minValue) {
            this->minValue = pixel;
        }

        if (this->maxValue == COLOR_WHITE_ORIGINAL &&
            this->minValue == COLOR_BLACK) {
            break;
        }
    }

    diversity = std::max(this->maxValue, this->minValue) -
                std::min(this->minValue, this->maxValue);
}

void Image::cut(RefImageLine srcImg) const {
    for (uint64_t col = 0; col < BLACK_COUNT; col++) {
        srcImg[col] = this->maxValue;
    }

    for (uint64_t col = LINE_LENGTH - 1; col > LINE_LENGTH - 1 - BLACK_COUNT;
         col--) {
        srcImg[col] = this->maxValue;
    }
}

void Image::normalize(RefCImageLine srcImg, RefImageLine dstImg) const {
    for (int i = 0; i < LINE_LENGTH; i++) {
        auto pixel = static_cast<float>(srcImg[i]);
        pixel -= this->minValue;
        pixel *= COLOR_WHITE;
        pixel /= (this->maxValue - this->minValue);
        dstImg[i] = static_cast<uint16_t>(pixel);
    }
}

uint16_t Image::averageThreshold(RefCImageLine srcImg) {
    long sum = 0;
    for (uint64_t i = BLACK_COUNT; i < LINE_LENGTH - BLACK_COUNT; i++) {
        sum += srcImg[i];
    }
    auto avg = static_cast<uint16_t>(sum / LINE_LENGTH - (2 * BLACK_COUNT));
    return avg;
}

void Image::threshold(RefCImageLine srcImg, RefImageLine dstImg) const {
    for (int i = 0; i < LINE_LENGTH; i++) {
        if (srcImg[i] < this->threshValue)
            dstImg[i] = COLOR_BLACK;
        else
            dstImg[i] = COLOR_WHITE;
    }
}

void Image::fastMedianBlur(RefImageLine srcImg, int pixels) {
    const int buffSize = pixels * 2 + 1;
    std::vector<uint16_t> blurBuffer;

    for (int i = pixels; i < LINE_LENGTH - pixels; i += buffSize) {
        for (int j = -pixels; j <= pixels; j++) {
            blurBuffer.emplace_back(srcImg[i + j]);
        }

        for (int j = -pixels; j <= pixels; j++) {
            srcImg[i + j] = Utils::median(blurBuffer);
        }
        blurBuffer.clear();
    }
}

void Image::fastMedianBlur(RefCImageLine srcImg, RefImageLine dstImg,
                           int pixels) {
    memcpy(dstImg, srcImg, LINE_LENGTH);

    const int buffSize = pixels * 2 + 1;
    std::vector<uint16_t> blurBuffer;

    for (int i = ((LINE_LENGTH / 2) % pixels) + pixels;
         i < LINE_LENGTH - pixels; i += buffSize) {
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

void Image::slowMedianBlur(RefCImageLine srcImg, RefImageLine dstImg,
                           int pixels) {
    memcpy(dstImg, srcImg, LINE_LENGTH);
    std::vector<uint16_t> blurBuffer;

    for (int i = pixels; i < LINE_LENGTH - pixels; i++) {
        for (int j = -pixels; j <= pixels; j++) {
            blurBuffer.emplace_back(srcImg[i - j]);
        }
        std::sort(blurBuffer.begin(), blurBuffer.end());
        dstImg[i] = blurBuffer.at(pixels + 1);
        blurBuffer.clear();
    }
}

void Image::slowMedianBlur(RefImageLine srcImg, int pixels) {
    std::deque<uint16_t> pixBuffer;

    for (int i = /*0*/ 1; i < LINE_LENGTH; i++) {
        if (static_cast<int>(pixBuffer.size()) < pixels) {
            pixBuffer.emplace_back(srcImg[i]);
            continue;
        }
        pixBuffer.emplace_back(srcImg[i]);

        for (int j = -pixels; j <= pixels; j++) {
            srcImg[i + j] = Utils::median<uint16_t>(
                std::vector<uint16_t>(pixBuffer.begin(), pixBuffer.end()));
        }
        // blurBuffer.clear();
        pixBuffer.pop_front();
    }
}

uint8_t Image::atThresh(uint8_t index) const {
    if (index >= LINE_LENGTH) {
        return 0;
    }

    return static_cast<uint8_t>(this->thresholdedImage[index]);
}

uint16_t Image::getMin() const { return this->minValue; }

uint16_t Image::getMax() const { return this->maxValue; }

uint16_t Image::getThreshValue() const { return this->threshValue; }

int16_t Image::getDiversity() const { return this->diversity; }

bool Image::isLowDiversity() const { return this->diversity < LOW_DIVERSITY; }

Image::Image(uint16_t (&rawImage)[LINE_LENGTH]) : Image() {
	uint16_t blurImage[LINE_LENGTH] = {0};
	fastMedianBlur(rawImage, blurImage, 1);
	std::memcpy(rawImage, blurImage, sizeof(rawImage));

    this->computeMinMax(rawImage);
	this->cut(rawImage);

	this->normalize(rawImage, this->thresholdedImage);

	this->threshValue = this->averageThreshold(this->thresholdedImage);
	this->threshold(this->thresholdedImage, this->thresholdedImage);
}

uint16_t Image::at(uint8_t index, Type type) const {
    if (index >= LINE_LENGTH) {
        return 0;
    }

    switch (type) {
    case Thresholded:
        return this->thresholdedImage[index];
    default:
        return 0;
    }
}
