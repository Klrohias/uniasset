//
// Created by Twiiz on 2024/1/14.
//

#ifndef UNIASSET_IMAGEASSET_HPP
#define UNIASSET_IMAGEASSET_HPP

#include <cstdint>
#include <string_view>
#include "uniasset/Foundation.hpp"
#include "uniasset/common/ErrorHandler.hpp"

namespace Uniasset {

    class UNIASSET_API ImageAsset {
    private:
        enum BufferSource {
            Self,
            Stb,
        };

        struct Buffer {
            uint8_t* buffer{nullptr};
            BufferSource source{Self};
        };

        Buffer buffer_{nullptr, Self};
        int32_t width_{-1};
        int32_t height_{-1};
        int32_t channelCount_{0};
        ErrorHandler errorHandler_{};

        static void ReleaseBuffer(Buffer& buffer);

        static Buffer AllocateBuffer(size_t size);

        bool LoadWebPFromFile(const char* path);

        bool LoadJpegFromFile(const char* path);

        bool LoadFromFile(const char* path);

        bool LoadWebP(uint8_t* fileData, size_t size);

        bool LoadJpeg(uint8_t* fileData, size_t size);

        bool LoadFile(uint8_t* fileData, size_t size);

    public:
        explicit ImageAsset();

        ~ImageAsset();

        bool Load(const std::string_view& path);

        bool Load(uint8_t* pixelData, size_t size, int32_t width, int32_t height, int32_t channelCount);

        bool Load(uint8_t* fileData, size_t size);

        const std::string& GetError();

        int32_t GetWidth() const;

        int32_t GetHeight() const;

        int32_t GetChannelCount() const;
    };

} // Uniasset

#endif //UNIASSET_IMAGEASSET_HPP
