//
// Created by qingy on 2024/3/10.
//

#ifndef UNIASSET_IMAGEPROCESSOR_HPP
#define UNIASSET_IMAGEPROCESSOR_HPP

#include <memory>
#include "uniasset/Foundation.hpp"
#include "uniasset/image/Pixels.hpp"
#include "uniasset/common/ErrorHandler.hpp"

namespace uniasset {
    using namespace utils;

    class ImageAsset;
    class UNIASSET_API ImageProcessor {
        std::shared_ptr<ImageAsset> asset_{};
        ErrorHandler errorHandler_{};

    public:
        explicit ImageProcessor(std::shared_ptr<ImageAsset> asset);

        [[nodiscard]] const std::string& getError() const;

        void drawPoint(int32_t x, int32_t y, Pixel24 color);

        void drawPoint(int32_t x, int32_t y, Pixel32 color);

        void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t width);

        void clear(Pixel24 color);

        void clear(Pixel32 color);
    };

} // uniasset

#endif //UNIASSET_IMAGEPROCESSOR_HPP
