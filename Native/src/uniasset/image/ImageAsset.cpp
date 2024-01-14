//
// Created by Twiiz on 2024/1/14.
//

#include <cstdio>
#include <memory>
#include <stb_image.h>
#include <webp/decode.h>
#include <turbojpeg.h>

#include "ImageAsset.hpp"
#include "../common/Utils.hpp"

namespace Uniasset {
    void TurboJpegHandleDeleter(tjhandle obj) {
        if (!obj) return;
        tjDestroy(obj);
    }

    void TurboJpegMemoryDeleter(unsigned char* obj) {
        if (!obj) return;
        tjFree(obj);
    }


    auto ImageAsset::AllocateBuffer(size_t size) -> Buffer {
        if (!size) {
            return {nullptr, Self};
        }

        return {new uint8_t[size], Self};
    }


    void ImageAsset::ReleaseBuffer(Buffer& buffer) {
        if (!buffer.buffer) return;
        switch (buffer.source) {
            case Self:
                delete[] buffer.buffer;
                break;
            case Stb:
                stbi_image_free(buffer.buffer);
                break;
        }

        buffer.buffer = nullptr;
    }

    ImageAsset::ImageAsset() = default;

    ImageAsset::~ImageAsset() {
        ReleaseBuffer(buffer_);
    }

    bool ImageAsset::Load(const std::string_view& path) {
        // Open
        FILE* fileRaw = fopen(path.data(), "rb");
        if (!fileRaw) {
            errorHandler_.SetError(strerror(errno));
            return false;
        }
        CFilePtr file(fileRaw, Utils::CFileDeleter);

        // Detect
        if (fseek(file.get(), 0L, SEEK_END) != 0) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to detect format (seek to end)");
            return false;
        }

        size_t fileSize = ftell(file.get());
        if (!fileSize) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to detect format (empty file)");
            return false;
        }

        if (fseek(file.get(), 0L, SEEK_SET)) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to detect format (seek to begin)");
            return false;
        }

        uint8_t magicNumberBuffer[16];
        size_t readSize = fileSize > sizeof(magicNumberBuffer) ? sizeof(magicNumberBuffer) : fileSize;
        if (!fread(magicNumberBuffer, readSize, 1, file.get())) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to detect format (read file)");
            return false;
        }

        file.reset(nullptr);

        // Load
        if (Utils::IsWebPFileData(magicNumberBuffer, readSize)) {
            return LoadWebPFromFile(path.data());
        }

        if (Utils::IsJpegFileData(magicNumberBuffer, readSize)) {
            return LoadJpegFromFile(path.data());
        }

        return LoadFromFile(path.data());
    }

    const std::string& ImageAsset::GetError() {
        return errorHandler_.GetError();
    }

    bool ImageAsset::Load(uint8_t* pixelData, size_t size, int32_t width, int32_t height, int32_t channelCount) {
        buffer_ = AllocateBuffer(size);
        memcpy(buffer_.buffer, pixelData, size);
        width_ = width;
        height_ = height;
        channelCount_ = channelCount;

        return true;
    }

    bool ImageAsset::Load(uint8_t* fileData, size_t size) {
        // Load
        if (Utils::IsWebPFileData(fileData, size)) {
            return LoadWebP(fileData, size);
        }

        if (Utils::IsJpegFileData(fileData, size)) {
            return LoadJpeg(fileData, size);
        }

        return LoadFile(fileData, size);
    }

    bool ImageAsset::LoadFromFile(const char* path) {
        stbi_set_flip_vertically_on_load(true);

        auto* data = stbi_load(path, &width_, &height_, &channelCount_, 0);
        if (!data) {
            errorHandler_.SetError(stbi_failure_reason());
            return false;
        }

        ReleaseBuffer(buffer_);
        buffer_ = {data, Stb};
        return true;
    }

    bool ImageAsset::LoadFile(uint8_t* fileData, size_t size) {
        stbi_set_flip_vertically_on_load(true);

        uint8_t* data = stbi_load_from_memory(fileData, size, &width_, &height_, &channelCount_, 0);
        if (!data) {
            errorHandler_.SetError(stbi_failure_reason());
            return false;
        }

        ReleaseBuffer(buffer_);
        buffer_ = {data, Stb};
        return true;
    }

    bool ImageAsset::LoadWebP(uint8_t* fileData, size_t size) {
        if (!WebPGetInfo(fileData, size, &width_, &height_)) {
            errorHandler_.SetError("Failed to get webp info");
            return false;
        }

        channelCount_ = 4;

        std::unique_ptr<uint8_t> buffer{new uint8_t[width_ * height_ * channelCount_]};
        VP8StatusCode statusCode;
        WebPDecoderConfig decoderConfig;

        if (WebPInitDecoderConfig(&decoderConfig) == 0) {
            errorHandler_.SetError("Failed to initialize decoder config");
            return false;
        }

        decoderConfig.options.use_threads = true;
        decoderConfig.options.use_scaling = false;
        if ((statusCode = WebPGetFeatures(fileData, size, &decoderConfig.input)) != VP8_STATUS_OK) {
            errorHandler_.SetError("Failed to call WebPGetFeatures: " + std::to_string(statusCode));
            return false;
        }

        auto strideSize = channelCount_ * width_;

        decoderConfig.output.is_external_memory = true;
        decoderConfig.output.colorspace = MODE_RGBA;
        decoderConfig.output.u.RGBA.stride = -strideSize;
        decoderConfig.output.u.RGBA.rgba = buffer.get() + (height_ - 1) * strideSize;
        decoderConfig.output.u.RGBA.size = height_ * strideSize;
        decoderConfig.output.width = width_;
        decoderConfig.output.height = height_;

        if ((statusCode = WebPDecode(fileData, size, &decoderConfig)) != VP8_STATUS_OK) {
            errorHandler_.SetError("Failed to call WebPDecode: " + std::to_string(statusCode));
            return false;
        }

        ReleaseBuffer(buffer_);
        buffer_ = {buffer.release(), Self};

        return true;
    }

    bool ImageAsset::LoadWebPFromFile(const char* path) {
        // Open
        FILE* fileRaw = fopen(path, "rb");
        if (!fileRaw) {
            errorHandler_.SetError(strerror(errno));
            return false;
        }
        CFilePtr file(fileRaw, Utils::CFileDeleter);

        // Read
        if (fseek(file.get(), 0L, SEEK_END) != 0) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load webp (seek to end)");
            return false;
        }

        size_t fileSize = ftell(file.get());
        if (!fileSize) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load webp (empty file)");
            return false;
        }

        if (fseek(file.get(), 0L, SEEK_SET)) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load webp (seek to begin)");
            return false;
        }

        std::unique_ptr<uint8_t> rdBuf(new uint8_t[fileSize]);
        if (!fread(rdBuf.get(), fileSize, 1, file.get())) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load webp (read file)");
            return false;
        }

        return LoadWebP(rdBuf.get(), fileSize);
    }

    bool ImageAsset::LoadJpegFromFile(const char* path) {
        // Open
        FILE* fileRaw = fopen(path, "rb");
        if (!fileRaw) {
            errorHandler_.SetError(strerror(errno));
            return false;
        }
        CFilePtr file(fileRaw, Utils::CFileDeleter);

        // Read
        if (fseek(file.get(), 0L, SEEK_END) != 0) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load jpeg (seek to end)");
            return false;
        }

        size_t fileSize = ftell(file.get());
        if (!fileSize) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load jpeg (empty file)");
            return false;
        }

        if (fseek(file.get(), 0L, SEEK_SET)) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load jpeg (seek to begin)");
            return false;
        }

        std::unique_ptr<uint8_t> rdBuf(new uint8_t[fileSize]);
        if (!fread(rdBuf.get(), fileSize, 1, file.get())) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load jpeg (read file)");
            return false;
        }

        return LoadJpeg(rdBuf.get(), fileSize);
    }

    bool ImageAsset::LoadJpeg(uint8_t* fileData, size_t size) {
        using ProcessorPtr = std::unique_ptr<void, decltype(TurboJpegHandleDeleter)*>;
        using MemoryPtr = std::unique_ptr<uint8_t, decltype(TurboJpegMemoryDeleter)*>;
        ProcessorPtr processor(nullptr, TurboJpegHandleDeleter);
        MemoryPtr transformedData(nullptr, TurboJpegMemoryDeleter);
        size_t transformedSize = 0;

        // Transform
        {
            auto processorRaw = tjInitTransform();
            if (!processorRaw) {
                errorHandler_.SetError(tjGetErrorStr());
                return false;
            }

            processor.reset(processorRaw);
        }
        {
            tjtransform transform{};
            memset(&transform, 0, sizeof(tjtransform));
            transform.op = TJXOP_VFLIP;

            uint8_t* transformedDataRaw = nullptr;
            if (tjTransform(processor.get(), fileData, size, 1, &transformedDataRaw,
                            &transformedSize, &transform, 0) != 0) {
                errorHandler_.SetError(tjGetErrorStr2(processor.get()));
                return false;
            }

            transformedData.reset(transformedDataRaw);
        }

        // Decode
        {
            auto processorRaw = tjInitDecompress();
            if (!processorRaw) {
                errorHandler_.SetError(tjGetErrorStr());
                return false;
            }

            processor.reset(processorRaw);
        }
        {
            int jpegSubsample, jpegColorspace;
            if (tjDecompressHeader3(processor.get(), fileData,
                                    size, &width_, &height_, &jpegSubsample, &jpegColorspace) != 0) {
                errorHandler_.SetError(tjGetErrorStr2(processor.get()));
                return false;
            }

            channelCount_ = 3;

            std::unique_ptr<uint8_t> buffer{new uint8_t[width_ * height_ * channelCount_]};
            if (tjDecompress2(processor.get(),
                              transformedData.get(),
                              transformedSize,
                              buffer.get(), width_,
                              0,
                              height_,
                              TJPF_RGB,
                              TJFLAG_FASTDCT) != 0) {
                errorHandler_.SetError(tjGetErrorStr2(processor.get()));
                return false;
            }

            ReleaseBuffer(buffer_);
            buffer_ = {buffer.release(), Self};
        }

        return true;
    }

    int32_t ImageAsset::GetWidth() const {
        return width_;
    }

    int32_t ImageAsset::GetHeight() const {
        return height_;
    }

    int32_t ImageAsset::GetChannelCount() const {
        return channelCount_;
    }


} // Uniasset