//
// Created by qingy on 2024/3/10.
//

#include "ImageProcessor.hpp"

#include "ImageAsset.hpp"
#include "../common/Errors.hpp"
#include <utility>

namespace uniasset {
    ImageProcessor::ImageProcessor(std::shared_ptr<ImageAsset> asset)
            : asset_{std::move(asset)} {
    }

    const std::string& ImageProcessor::getError() const {
        return errorHandler_.getError();
    }

    void ImageProcessor::clear(Pixel32 color) {
        static_assert(sizeof(Pixel32) == 4);

        errorHandler_.clear();
        if (asset_->getChannelCount() != 4) {
            errorHandler_.setError(ERROR_STR_IMAGE_CHANNEL_MISMATCH);
            return;
        }


    }

    void ImageProcessor::clear(Pixel24 color) {
        static_assert(sizeof(Pixel24) == 3);

        errorHandler_.clear();
        if (asset_->getChannelCount() != 3) {
            errorHandler_.setError(ERROR_STR_IMAGE_CHANNEL_MISMATCH);
            return;
        }
    }

    void ImageProcessor::drawPoint(int32_t x, int32_t y, Pixel24 color) {
        errorHandler_.clear();

        auto width = asset_->getWidth();
        auto height = asset_->getHeight();

        if (x < 0 || y < 0 || x >= width || y >= height) return;

        auto channelCount = asset_->getChannelCount();
        auto buffer = asset_->getBuffer();
        auto scanLineSize = channelCount * width;

        if (channelCount == 3) {
            auto pixel = static_cast<Pixel24*>(ptr_offset(buffer, (height - 1 - y) * scanLineSize + x * channelCount));
            *pixel = color;

            return;
        } else if (channelCount == 4) {
            auto pixel = static_cast<Pixel32*>(ptr_offset(buffer, (height - 1 - y) * scanLineSize + x * channelCount));


            return;
        }
        errorHandler_.setError(ERROR_STR_IMAGE_CHANNEL_MISMATCH);
    }
} // uniasset