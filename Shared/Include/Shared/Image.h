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
    Image();

    Image(RefImageLine rawImage);

    virtual uint16_t at(uint8_t index, Type type) const;

    uint8_t atThresh(uint8_t index) const;

    uint16_t getMin() const;

    uint16_t getMax() const;

    uint16_t getThreshValue() const;

    int16_t getDiversity() const;

    bool isLowDiversity() const;

  protected:
    void computeMinMax(RefCImageLine img);

    void cut(RefImageLine srcImg) const;

    void normalize(RefCImageLine srcImg, RefImageLine dstImg) const;

    static uint16_t averageThreshold(RefCImageLine srcImg);

    void threshold(RefCImageLine srcImg, RefImageLine dstImg) const;

    static void slowMedianBlur(RefCImageLine srcImg, RefImageLine dstImg,
                               int pixels);

    static void slowMedianBlur(RefImageLine srcImg, int pixels);

    static void fastMedianBlur(RefCImageLine srcImg, RefImageLine dstImg,
                               int pixels);

    static void fastMedianBlur(RefImageLine srcImg, int pixels);
};
} // namespace Shared
#endif
