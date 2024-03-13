#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>

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

    explicit Image(ImageLine rawImage);

    [[nodiscard]] virtual uint16_t At(uint8_t index, Type type) const;

    [[nodiscard]] uint8_t AtThresh(uint8_t index) const;

    [[nodiscard]] uint16_t Min() const;

    [[nodiscard]] uint16_t Max() const;

    [[nodiscard]] uint16_t ThreshValue() const;

    [[nodiscard]] int16_t Diversity() const;

    [[nodiscard]] bool IsLowDiversity() const;

  protected:
    void ComputeMinMax(CImageLine img);

    void Cut(ImageLine srcImg) const;

    void Normalize(CImageLine srcImg, ImageLine dstImg) const;

    static uint16_t AverageThreshold(CImageLine srcImg);

    void Threshold(CImageLine srcImg, ImageLine dstImg) const;

    static void SlowMedianBlur(CImageLine srcImg, ImageLine dstImg, int pixels);

    static void SlowMedianBlur(ImageLine srcImg, int pixels);

    static void FastMedianBlur(CImageLine srcImg, ImageLine dstImg, int pixels);

    static void FastMedianBlur(ImageLine srcImg, int pixels);

  protected:
    uint16_t m_thresholdedImage[LINE_LENGTH];
    uint16_t m_max;
    uint16_t m_min;
    uint16_t m_threshValue{};
    int16_t m_diversity{};
};
} // namespace Shared
#endif
