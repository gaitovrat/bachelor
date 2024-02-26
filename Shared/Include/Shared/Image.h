#ifndef _IMAGE_H
#define _IMAGE_H

#include <stdint.h>

namespace Shared {
class Image {
  public:
    enum Type { Raw, Normalized, Thresholded };

    static constexpr uint8_t LINE_LENGTH = 128u;
    static constexpr uint8_t COLOR_BLACK = 0x00;
    static constexpr uint8_t COLOR_WHITE = 0xff;
    static constexpr uint16_t COLOR_WHITE_ORIGINAL = 0xfff;
    static constexpr uint8_t LOW_DIVERSITY = 100;
    static constexpr uint8_t BLACK_COUNT = 10;

    using ImageLine = uint16_t (&)[LINE_LENGTH];
    using CImageLine = const uint16_t (&)[LINE_LENGTH];

    Image();

    Image(ImageLine rawImage);

    uint16_t At(uint8_t index, Type type) const;

    uint8_t AtThresh(uint8_t index) const;

    uint16_t Min() const;

    uint16_t Max() const;

    uint16_t ThreshValue() const;

    int16_t Diversity() const;

    bool IsLowDiversity() const;

  protected:
    void ComputeMinMax(CImageLine img);

    void Cut(ImageLine srcImg);

    void Normalize(CImageLine srcImg, ImageLine dstImg);

    uint16_t AverageThreshold(CImageLine srcImg);

    void Threshold(CImageLine srcImg, ImageLine dstImg);

    void SlowMedianBlur(CImageLine srcImg, ImageLine dstImg, int pixels);

    void SlowMedianBlur(ImageLine srcImg, int pixels);

    void FastMedianBlur(CImageLine srcImg, ImageLine dstImg, int pixels);

    void FastMedianBlur(ImageLine srcImg, int pixels);

  protected:
    uint16_t m_thresholdedImage[LINE_LENGTH];
    uint16_t m_max;
    uint16_t m_min;
    uint16_t m_threshValue;
    int16_t m_diversity;
};
} // namespace Shared
#endif
