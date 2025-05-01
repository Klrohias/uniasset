//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_IMAGEASSET_HPP
#define UNIASSET_IMAGEASSET_HPP

#include <cstdint>
#include <string_view>
#include <functional>
#include <memory>
#include <span>
#include <vector>
#include "uniasset/Foundation.hpp"
#include "uniasset/common/Buffer.hpp"
#include "uniasset/common/Templates.hpp"
#include "uniasset/common/Result.hpp"

namespace uniasset {
    inline void stb_deleter(uint8_t* buffer);

    struct CropOptions {
        int32_t x, y, width, height;
    };

    class UNIASSET_API ImageAsset : public std::enable_shared_from_this<ImageAsset> {
    private:
        Buffer ringBuffer_{nullptr, default_array_deleter<uint8_t>};

        int32_t width_{-1};
        int32_t height_{-1};
        int32_t channelCount_{0};

        std::error_code loadWebP(const std::string_view& path);

        std::error_code loadJpeg(const std::string_view& path);

        std::error_code loadFile(const std::string_view& path);

        std::error_code loadWebP(uint8_t* fileData, size_t size);

        std::error_code loadJpeg(uint8_t* fileData, size_t size);

        std::error_code loadFile(uint8_t* fileData, size_t size);

    public:
        explicit ImageAsset();

        explicit ImageAsset(Buffer&& buffer, int32_t width, int32_t height, int32_t channelCount);

        ImageAsset(ImageAsset&&) = default;

        std::error_code load(const std::string_view& path);

        std::error_code load(uint8_t* pixelData, size_t size, int32_t width, int32_t height, int32_t channelCount);

        std::error_code load(uint8_t* fileData, size_t size);

        std::error_code unload();

        Result<int32_t> getWidth();

        Result<int32_t> getHeight();

        Result<int32_t> getChannelCount();

        std::error_code crop(int32_t x, int32_t y, int32_t width, int32_t height);

        std::error_code resize(int32_t width, int32_t height);

        std::error_code copyTo(void* buffer);

        [[nodiscard]] Result<ImageAsset*> clone() const;

        Result<std::vector<ImageAsset>> cropMultiple(std::span<CropOptions> items);
    };

} // Uniasset

#endif //UNIASSET_IMAGEASSET_HPP
