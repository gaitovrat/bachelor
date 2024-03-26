//
// Created by ratmirgaitov on 6/14/23.
//

#ifndef CARCONTROL_IMAGE_H
#define CARCONTROL_IMAGE_H

#include "Shared/Image.h"

namespace CarQt {
struct Image : public Shared::Image {
    ImageLine rawImage;
    ImageLine normalizedImage;
    ImageLine thresholdedImage;

    Image();

    explicit Image(RefCImageLine rawImage);

    explicit Image(RefImageLine rawImage);

    void setRawImage(RefCImageLine rawImage);

    void process();

    uint16_t at(uint8_t index, Type type) const override;
};
} // namespace CarQt

#endif // CARCONTROL_IMAGE_H
