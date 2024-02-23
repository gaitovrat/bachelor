//
// Created by ratmirgaitov on 6/14/23.
//

#include <cstring>
#include <iostream>

#include "Image.h"

Client::Image::Image() : RawImage{0}, NormalizedImage{0}, ThresholdedImage{0} {}

Client::Image::Image(uint16_t (&rawImage)[128]) : ::Image() {
    SetRawImage(rawImage);
}

void Client::Image::SetRawImage(uint16_t (&rawImage)[128]) {
    std::memcpy(RawImage, rawImage, sizeof(rawImage));
    Process();
}

void Client::Image::Process() {
    ComputeMinMax(RawImage);
    Cut(RawImage);
    Normalize(RawImage, NormalizedImage);
    m_threshValue = AverageThreshold(NormalizedImage);
    Threshold(NormalizedImage, ThresholdedImage);
}

uint16_t Client::Image::At(const uint8_t index, ImgType type) const {
    if (index >= TFC_CAMERA_LINE_LENGTH) {
        std::cerr << "Index out of bounds\n";
        return 0;
    }

    switch (type) {
        case ImgType::Raw:
            return RawImage[index];
        case ImgType::Normalized:
            return NormalizedImage[index];
        case ImgType::Thresholded:
            return ThresholdedImage[index];
        default:
            std::cerr << "Unknown image type\n";
            return 0;
    }
}
