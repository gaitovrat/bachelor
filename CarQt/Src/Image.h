//
// Created by ratmirgaitov on 6/14/23.
//

#ifndef CARCONTROL_IMAGE_H
#define CARCONTROL_IMAGE_H

#include "Shared/Image.h"

namespace CarQt {
struct Image : public Shared::Image {
    uint16_t rawImage[LINE_LENGTH];
    uint16_t normalizedImage[LINE_LENGTH];
    uint16_t thresholdedImage[LINE_LENGTH];

    Image();

    explicit Image(CImageLine rawImage);

    explicit Image(ImageLine rawImage);

    void setRawImage(CImageLine rawImage);

    void process();

    uint16_t at(uint8_t index, Type type) const override;
};
} // namespace CarQt

#endif // CARCONTROL_IMAGE_H
