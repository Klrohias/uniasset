//
// Created by Twiiz on 2024/1/14.
//

#include <cstdio>
#include <memory>
#include <stb_image.h>
#include <webp/decode.h>
#include <cstring>

#include "ImageAsset.hpp"
#include "../thirdparty/unique.hpp"
#include "../thirdparty/c_file.hpp"
#include "../thirdparty/turbojpeg.hpp"
#include "../common/Templates.hpp"
#include "../common/MagicNumbers.hpp"
#include "../common/Errors.hpp"

namespace uniasset {
    template<int PixelSize = 3>
    void ScaleImage(const uint8_t* srcBuffer,
                    uint8_t* destBuffer,
                    uint32_t srcWidth,
                    uint32_t srcHeight,
                    uint32_t destWidth,
                    uint32_t destHeight) {
        float scaleX, scaleY;
        scaleX = static_cast<float>(srcWidth) / static_cast<float>(destWidth);
        scaleY = static_cast<float>(srcHeight) / static_cast<float>(destHeight);

        for (int ix = 0; ix < destWidth; ix++) {
            for (int iy = 0; iy < destHeight; iy++) {
                int px, py;
                px = static_cast<int> (static_cast<float>(ix) * scaleX);
                py = static_cast<int> (static_cast<float>(iy) * scaleY);
                uint32_t destPixelBegin = destWidth * iy + ix;
                uint32_t srcPixelBegin = srcWidth * py + px;
                static_for<0, PixelSize>()([&](int i) {
                    destBuffer[destPixelBegin * PixelSize + i] = srcBuffer[srcPixelBegin * PixelSize + i];
                });
            }
        }
    }

    ImageAsset::ImageAsset() = default;

    ImageAsset::ImageAsset(Buffer&& buffer, int32_t width, int32_t height, int32_t channelCount)
            : ringBuffer_{std::move(buffer)}, width_{width}, height_{height}, channelCount_{channelCount} {
    }

    std::error_code ImageAsset::load(const std::string_view& path) {
        // open
        auto file{make_c_unique<FILE, FILE_deleter>(fopen(path.data(), "rb"))};
        if (!file) {
            return err_errno();
        }

        // detect
        if (fseek(file.get(), 0L, SEEK_END) != 0) {
            return err_errno();
        }

        size_t fileSize = ftell(file.get());
        if (!fileSize) {
            return err_errno();
        }

        if (fseek(file.get(), 0L, SEEK_SET)) {
            return err_errno();
        }

        uint8_t magicNumberBuffer[16];
        size_t readSize = fileSize > sizeof(magicNumberBuffer) ? sizeof(magicNumberBuffer) : fileSize;
        if (!fread(magicNumberBuffer, readSize, 1, file.get())) {
            return err_errno();
        }

        file.reset(nullptr);

        // load
        if (isWebPFileData(magicNumberBuffer, readSize)) {
            return loadWebP(path.data());
        } else if (isJpegFileData(magicNumberBuffer, readSize)) {
            return loadJpeg(path.data());
        } else {
            return loadFile(path.data());
        }
    }

    std::error_code
    ImageAsset::load(uint8_t* pixelData, size_t size, int32_t width, int32_t height, int32_t channelCount) {
        ringBuffer_ = {new uint8_t[size], default_array_deleter<uint8_t>};
        memcpy(ringBuffer_.get(), pixelData, size);

        width_ = width;
        height_ = height;
        channelCount_ = channelCount;

        return err_ok();
    }

    std::error_code ImageAsset::load(uint8_t* fileData, size_t size) {
        // load
        if (isWebPFileData(fileData, size)) {
            return loadWebP(fileData, size);
        } else if (isJpegFileData(fileData, size)) {
            return loadJpeg(fileData, size);
        } else {
            return loadFile(fileData, size);
        }
    }

    std::error_code ImageAsset::loadFile(const std::string_view& path) {
        stbi_set_flip_vertically_on_load(true);
        ringBuffer_ = {stbi_load(path.data(), &width_, &height_, &channelCount_, 0), stb_deleter};
        if (!ringBuffer_) {
            return {1, stbi_category()};
        }

        return err_ok();
    }

    std::error_code ImageAsset::loadFile(uint8_t* fileData, size_t size) {
        stbi_set_flip_vertically_on_load(true);

        ringBuffer_ = {stbi_load_from_memory(fileData, size, &width_, &height_, &channelCount_, 0), stb_deleter};
        if (!ringBuffer_) {
            return {1, stbi_category()};
        }

        return err_ok();
    }

    std::error_code ImageAsset::loadWebP(uint8_t* fileData, size_t size) {
        if (!WebPGetInfo(fileData, size, &width_, &height_)) {
            return {kWebPInfoReadFail, uniasset_category()};
        }

        channelCount_ = 4;

        ringBuffer_ = {new uint8_t[width_ * height_ * channelCount_], default_array_deleter<uint8_t>};
        VP8StatusCode statusCode;
        WebPDecoderConfig decoderConfig;

        if (WebPInitDecoderConfig(&decoderConfig) == 0) {
            return {kWebPDecoderInitFail, uniasset_category()};
        }

        decoderConfig.options.use_threads = true;
        decoderConfig.options.use_scaling = false;
        if ((statusCode = WebPGetFeatures(fileData, size, &decoderConfig.input)) != VP8_STATUS_OK) {
            return {statusCode, vp8_category()};
        }

        auto strideSize = channelCount_ * width_;

        decoderConfig.output.is_external_memory = true;
        decoderConfig.output.colorspace = MODE_RGBA;
        decoderConfig.output.u.RGBA.stride = -strideSize;
        decoderConfig.output.u.RGBA.rgba = ringBuffer_.get() + (height_ - 1) * strideSize;
        decoderConfig.output.u.RGBA.size = height_ * strideSize;
        decoderConfig.output.width = width_;
        decoderConfig.output.height = height_;

        if ((statusCode = WebPDecode(fileData, size, &decoderConfig)) != VP8_STATUS_OK) {
            return {statusCode, vp8_category()};
        }

        return err_ok();
    }

    std::error_code ImageAsset::loadWebP(const std::string_view& path) {
        // open
        auto file{make_c_unique<FILE, FILE_deleter>(fopen(path.data(), "rb"))};
        if (!file) {
            return err_errno();
        }

        // read
        if (fseek(file.get(), 0L, SEEK_END) != 0) {
            return err_errno();
        }

        size_t fileSize = ftell(file.get());
        if (!fileSize) {
            return err_errno();
        }

        if (fseek(file.get(), 0L, SEEK_SET)) {
            return err_errno();
        }

        std::unique_ptr<uint8_t> rdBuf(new uint8_t[fileSize]);
        if (!fread(rdBuf.get(), fileSize, 1, file.get())) {
            return err_errno();
        }

        return loadWebP(rdBuf.get(), fileSize);
    }

    std::error_code ImageAsset::loadJpeg(const std::string_view& path) {
        // open
        auto file{make_c_unique<FILE, FILE_deleter>(fopen(path.data(), "rb"))};
        if (!file) {
            return err_errno();
        }

        // read
        if (fseek(file.get(), 0L, SEEK_END) != 0) {
            return err_errno();
        }

        size_t fileSize = ftell(file.get());
        if (!fileSize) {
            return err_errno();
        }

        if (fseek(file.get(), 0L, SEEK_SET)) {
            return err_errno();
        }

        std::unique_ptr<uint8_t> rdBuf(new uint8_t[fileSize]);
        if (!fread(rdBuf.get(), fileSize, 1, file.get())) {
            return err_errno();
        }

        return loadJpeg(rdBuf.get(), fileSize);
    }

    std::error_code ImageAsset::loadJpeg(uint8_t* fileData, size_t size) {
        c_unique_ptr<void, tj_deleter> processor{nullptr, tj_deleter};

        // decode
        processor.reset(tjInitDecompress());
        if (!processor) {
            return {kTurboJpegInitFail, uniasset_category()};
        }

        int jpegSubsample, jpegColorspace;
        if (tjDecompressHeader3(processor.get(), fileData,
                                size, &width_, &height_, &jpegSubsample, &jpegColorspace) != 0) {
            return {tjGetErrorCode(processor.get()), turbojpeg_category()};
        }

        channelCount_ = 3;

        Buffer buffer{new uint8_t[width_ * height_ * channelCount_], default_array_deleter<uint8_t>};
        if (tjDecompress2(processor.get(),
                          fileData,
                          size,
                          buffer.get(), width_,
                          0,
                          height_,
                          TJPF_RGB,
                          TJFLAG_FASTDCT | TJFLAG_BOTTOMUP) != 0) {
            return {tjGetErrorCode(processor.get()), turbojpeg_category()};
        }

        ringBuffer_ = std::move(buffer);

        return err_ok();
    }

    Result<int32_t> ImageAsset::getWidth() {
        if (!ringBuffer_) {
            return std::error_code(kImageNotLoadFail, uniasset_category());
        }

        return width_;
    }

    Result<int32_t> ImageAsset::getHeight() {
        if (!ringBuffer_) {
            return std::error_code(kImageNotLoadFail, uniasset_category());
        }

        return height_;
    }

    Result<int32_t> ImageAsset::getChannelCount() {
        if (!ringBuffer_) {
            return std::error_code(kImageNotLoadFail, uniasset_category());
        }

        return channelCount_;
    }

    std::error_code ImageAsset::crop(int32_t x, int32_t y, int32_t width, int32_t height) {
        if (!ringBuffer_) {
            return {kImageNotLoadFail, uniasset_category()};
        }

        int32_t srcStrideSize = width_ * channelCount_;
        int32_t newStrideSize = width * channelCount_;

        int32_t startLine = height_ - y - height;
        int32_t endLine = height_ - y;
        int32_t startPixel = x;
        int32_t endPixel = x + width;

        if (startLine < 0 || startLine > height_ ||
            endLine < 0 || endLine > height_ ||
            startPixel < 0 || startPixel > width_ ||
            endPixel < 0 || endPixel > width_) {
            return {kRectOverflow, uniasset_category()};
        }

        Buffer newBuffer{new uint8_t[width * height * channelCount_], default_array_deleter<uint8_t>};

        for (int iy = 0; iy < height; iy++) {
            memcpy(newBuffer.get() + newStrideSize * iy,
                   ringBuffer_.get() + srcStrideSize * (iy + startLine) + x * channelCount_, newStrideSize);
        }

        ringBuffer_ = std::move(newBuffer);
        width_ = width;
        height_ = height;

        return err_ok();
    }

    std::error_code ImageAsset::resize(int32_t width, int32_t height) {
        if (!ringBuffer_) {
            return {kImageNotLoadFail, uniasset_category()};
        }

        Buffer newBuffer{new uint8_t[width * height * channelCount_], default_array_deleter<uint8_t>};

        switch (channelCount_) {
            case 3: {
                ScaleImage<3>(ringBuffer_.get(), newBuffer.get(), width_, height_, width, height);
                break;
            }
            case 4: {
                ScaleImage<4>(ringBuffer_.get(), newBuffer.get(), width_, height_, width, height);
                break;
            }
            default:
                break;
        }

        width_ = width;
        height_ = height;

        ringBuffer_ = std::move(newBuffer);

        return err_ok();
    }

    std::error_code ImageAsset::unload() {
        if (!ringBuffer_) {
            return {kImageNotLoadFail, uniasset_category()};
        }

        ringBuffer_.reset();
        width_ = 0;
        height_ = 0;
        channelCount_ = 0;

        return err_ok();
    }

    std::error_code ImageAsset::copyTo(void* buffer) {
        if (!ringBuffer_) {
            return {kImageNotLoadFail, uniasset_category()};
        }

        memcpy(buffer, ringBuffer_.get(), width_ * height_ * channelCount_);

        return err_ok();
    }

    Result<ImageAsset*> ImageAsset::clone() const {
        if (!ringBuffer_) {
            return std::error_code{kImageNotLoadFail, uniasset_category()};
        }

        auto result = std::make_unique<ImageAsset>();

        if (auto err =
                    result->load(ringBuffer_.get(),
                                 width_ * height_ * channelCount_,
                                 width_,
                                 height_,
                                 channelCount_);
                err.value()) {
            return err;
        }

        return result.release();
    }

    Result<std::vector<ImageAsset>> ImageAsset::cropMultiple(std::span<CropOptions> items) {
        std::vector<ImageAsset> result{};

        // check
        for (const auto& [x, y, width, height]: items) {
            int32_t startLine = height_ - y - height;
            int32_t endLine = height_ - y;
            int32_t startPixel = x;
            int32_t endPixel = x + width;

            if (startLine < 0 || startLine > height_ ||
                endLine < 0 || endLine > height_ ||
                startPixel < 0 || startPixel > width_ ||
                endPixel < 0 || endPixel > width_) {

                return std::error_code{kRectOverflow, uniasset_category()};
            }
        }

        result.reserve(items.size());

        // crop
        for (const auto& [x, y, width, height]: items) {
            int32_t startLine = height_ - y - height;

            int32_t srcStrideSize = width_ * channelCount_;
            int32_t newStrideSize = width * channelCount_;

            Buffer newBuffer{new uint8_t[width * height * channelCount_], default_array_deleter<uint8_t>};

            for (int iy = 0; iy < height; iy++) {
                memcpy(newBuffer.get() + newStrideSize * iy,
                       ringBuffer_.get() + srcStrideSize * (iy + startLine) + x * channelCount_, newStrideSize);
            }

            result.emplace_back(std::move(newBuffer), width, height, channelCount_);
        }

        return result;
    }

    void stb_deleter(uint8_t* buffer) {
        stbi_image_free(buffer);
    }
} // Uniasset