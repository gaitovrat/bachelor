#include "Image.h"

#include <cstring>
#include <iostream>

using namespace CarQt;

Image::Image() : rawImage{0}, normalizedImage{0}, thresholdedImage{0} {}

Image::Image(RefCImageLine rawImage) : Image() { setRawImage(rawImage); }

Image::Image(RefImageLine rawImage)
    : Image(const_cast<RefCImageLine>(rawImage)) {}

void Image::setRawImage(RefCImageLine rawImage) {
    std::memcpy(this->rawImage, rawImage, sizeof(rawImage));
    process();
}

void Image::process() {
    computeMinMax(rawImage);
    cut(rawImage);
    normalize(rawImage, normalizedImage);
    threshValue = averageThreshold(normalizedImage);
    threshold(normalizedImage, thresholdedImage);
}

uint16_t Image::at(const uint8_t index, Type type) const {
    if (index >= LINE_LENGTH) {
        std::cerr << "Index out of bounds\n";
        return 0;
    }

    switch (type) {
        case Raw:
            return rawImage[index];
        case Normalized:
            return normalizedImage[index];
        case Thresholded:
            return thresholdedImage[index];
        default:
            std::cerr << "Unknown image type\n";
            return 0;
    }
}
