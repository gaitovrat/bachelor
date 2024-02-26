//
// Created by ratmirgaitov on 6/14/23.
//

#ifndef CARCONTROL_IMAGE_H
#define CARCONTROL_IMAGE_H

#include "Shared/Image.h"

namespace CarQt {
struct Image : public Shared::Image {
    Image();

    explicit Image(CImageLine rawImage);

    explicit Image(ImageLine rawImage);

    void SetRawImage(CImageLine rawImage);

    void Process();

    uint16_t At(const uint8_t index, Type type) const;

    uint16_t RawImage[LINE_LENGTH];
    uint16_t NormalizedImage[LINE_LENGTH];
    uint16_t ThresholdedImage[LINE_LENGTH];
};
} // namespace CarQt

#endif // CARCONTROL_IMAGE_H
