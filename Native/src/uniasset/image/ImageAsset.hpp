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
#include "uniasset/Foundation.hpp"
#include "uniasset/common/Buffer.hpp"
#include "uniasset/common/Templates.hpp"
#include "uniasset/common/ErrorHandler.hpp"

namespace uniasset {
    using namespace uniasset::utils;

    inline void stb_deleter(uint8_t* buffer);

    struct CropOptions {
        int32_t x, y, width, height;
    };

    class UNIASSET_API ImageAsset : public std::enable_shared_from_this<ImageAsset> {
    private:
        Buffer buffer_{nullptr, default_array_deleter<uint8_t>};

        ErrorHandler errorHandler_{};

        int32_t width_{-1};
        int32_t height_{-1};
        int32_t channelCount_{0};

        void loadWebP(const std::string_view& path);

        void loadJpeg(const std::string_view& path);

        void loadFile(const std::string_view& path);

        void loadWebP(uint8_t* fileData, size_t size);

        void loadJpeg(uint8_t* fileData, size_t size);

        void loadFile(uint8_t* fileData, size_t size);

    public:
        explicit ImageAsset();

        explicit ImageAsset(Buffer&& buffer, int32_t width, int32_t height, int32_t channelCount);

        ImageAsset(ImageAsset&&) = default;

        void load(const std::string_view& path);

        void load(uint8_t* pixelData, size_t size, int32_t width, int32_t height, int32_t channelCount);

        void load(int32_t width, int32_t height, int32_t channelCount);

        void load(uint8_t* fileData, size_t size);

        void unload();

        const std::string& getError();

        int32_t getWidth();

        int32_t getHeight();

        int32_t getChannelCount();

        void crop(int32_t x, int32_t y, int32_t width, int32_t height);

        void resize(int32_t width, int32_t height);

        void copyTo(void* buffer);

        [[nodiscard]] ImageAsset* clone() const;

        std::vector<ImageAsset> cropMultiple(std::span<CropOptions> items);

        uint8_t* getBuffer();
    };

} // Uniasset

#endif //UNIASSET_IMAGEASSET_HPP
