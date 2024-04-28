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

    /*
     * Constructor
     */
    Image();

    /*
     * Constructor
     * @param rawImage the raw image line
     */
    explicit Image(RefCImageLine rawImage);

    /*
     * Constructor
     * @param rawImage the raw image line
     */
    explicit Image(RefImageLine rawImage);

    /*
     * Set the raw image
     * @param rawImage the raw image line
     */
    void setRawImage(RefCImageLine rawImage);

    /*
     * Apply processing to the image
     */
    void process();

    /*
     * Get pixel at index of type
     * @param index the index of the pixel
     * @param type the type of the pixel (Raw, Normalized, Thresholded)
     * @return the pixel value
     */
    uint16_t at(uint8_t index, Type type) const override;
};
}  // namespace CarQt

#endif  // CARCONTROL_IMAGE_H
