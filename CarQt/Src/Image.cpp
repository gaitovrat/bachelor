#include <cstring>
#include <iostream>

#include "Image.h"

using namespace CarQt;

Image::Image() : RawImage{0}, NormalizedImage{0}, ThresholdedImage{0} {}

Image::Image(CImageLine rawImage) : Image() { SetRawImage(rawImage); }

Image::Image(ImageLine rawImage) : Image(const_cast<CImageLine>(rawImage)) {}

void Image::SetRawImage(const uint16_t (&rawImage)[128]) {
    std::memcpy(RawImage, rawImage, sizeof(rawImage));
    Process();
}

void Image::Process() {
    ComputeMinMax(RawImage);
    Cut(RawImage);
    Normalize(RawImage, NormalizedImage);
    m_threshValue = AverageThreshold(NormalizedImage);
    Threshold(NormalizedImage, ThresholdedImage);
}

uint16_t Image::At(const uint8_t index, Type type) const {
    if (index >= LINE_LENGTH) {
        std::cerr << "Index out of bounds\n";
        return 0;
    }

    switch (type) {
    case Raw:
        return RawImage[index];
    case Normalized:
        return NormalizedImage[index];
    case Thresholded:
        return ThresholdedImage[index];
    default:
        std::cerr << "Unknown image type\n";
        return 0;
    }
}
