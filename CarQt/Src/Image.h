//
// Created by ratmirgaitov on 6/14/23.
//

#ifndef CARCONTROL_IMAGE_H
#define CARCONTROL_IMAGE_H

#include "common/Image.h"
#include "common/ImageType.h"

namespace Client {
struct Image : public ::Image {
    Image();

    explicit Image(uint16_t (&rawImage)[TFC_CAMERA_LINE_LENGTH]);

    void SetRawImage(uint16_t (&rawImage)[TFC_CAMERA_LINE_LENGTH]);

    void Process();

    uint16_t At(const uint8_t index, ImgType type) const;

    uint16_t RawImage[TFC_CAMERA_LINE_LENGTH];
    uint16_t NormalizedImage[TFC_CAMERA_LINE_LENGTH];
    uint16_t ThresholdedImage[TFC_CAMERA_LINE_LENGTH];
};
}


#endif //CARCONTROL_IMAGE_H
