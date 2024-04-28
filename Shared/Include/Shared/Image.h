#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>

namespace Shared {
class Image {
  public:
    static constexpr uint8_t LINE_LENGTH = 128u;
    static constexpr uint8_t COLOR_BLACK = 0x00;
    static constexpr uint8_t COLOR_WHITE = 0xff;
    static constexpr uint16_t COLOR_WHITE_ORIGINAL = 0xfff;
    static constexpr uint8_t LOW_DIVERSITY = 100;
    static constexpr uint8_t BLACK_COUNT = 10;

    enum Type { Raw, Normalized, Thresholded };

    using RefImageLine = uint16_t (&)[LINE_LENGTH];
    using RefCImageLine = const uint16_t (&)[LINE_LENGTH];
    using ImageLine = uint16_t[LINE_LENGTH];

  protected:
    uint16_t thresholdedImage[LINE_LENGTH];
    uint16_t maxValue;
    uint16_t minValue;
    uint16_t threshValue;
    int16_t diversity;

public:
    /*
     * Constructor
     */
    Image();

    /*
     * Constructor
     * @param rawImage the raw image line
     */
    Image(RefImageLine rawImage);

    /*
     * Get pixel at index of type
     * @param index the index of the pixel
     * @param type the type of the pixel (Raw, Normalized, Thresholded)
     * @return the pixel value
     */
    virtual uint16_t at(uint8_t index, Type type) const;

    /*
     * Get thresholded pixel at index
     * @param index the index of the pixel
     * @return the pixel value
     */
    uint8_t atThresh(uint8_t index) const;

    /*
     * Get the minimum value of the image
     * @return the minimum value
     */
    uint16_t getMin() const;

    /*
     * Get the maximum value of the image
     * @return the minimum value
     */
    uint16_t getMax() const;

    /*
     * Get the threshold value of the image
     * @return the threshold value
     */
    uint16_t getThreshValue() const;

    /*
     * Get the diversity of the image
     * @return the diversity
     */
    int16_t getDiversity() const;

    /*
     * Check if the image has low diversity
     * @return true if the image has low diversity
     */
    bool isLowDiversity() const;

protected:
    /*
     * Compute the min and max values of the image
     * @param img the image line
     */
    void computeMinMax(RefCImageLine img);

    /*
     * Cut the image line
     * @param srcImg the source image line
     */
    void cut(RefImageLine srcImg) const;

    /*
     * Normalize the image line
     * @param srcImg the source image line
     * @param dstImg the destination image line
     */
    void normalize(RefCImageLine srcImg, RefImageLine dstImg) const;

    /*
     * Compute the average threshold of the image line
     * @param srcImg the source image line
     * @return the average threshold
     */
    uint16_t averageThreshold(RefCImageLine srcImg);

    /*
     * Threshold the image line
     * @param srcImg the source image line
     * @param dstImg the destination image line
     */
    void threshold(RefCImageLine srcImg, RefImageLine dstImg) const;

    /*
     * Apply median blur filter to the image line
     * @param srcImg the source image line
     * @param dstImg the destination image line
     * @param pixels the number of pixels to blur
     */
    void fastMedianBlur(RefCImageLine srcImg, RefImageLine dstImg, int pixels);
};
} // namespace Shared
#endif
