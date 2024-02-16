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
    using namespace uniasset::utils;

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

    void ImageAsset::load(const std::string_view& path) {
        errorHandler_.clear();

        // open
        auto file{make_c_unique<FILE, FILE_deleter>(fopen(path.data(), "rb"))};
        if (!file) {
            errorHandler_.setError(strerror(errno));
            return;
        }

        // detect
        if (fseek(file.get(), 0L, SEEK_END) != 0) {
            ERROR_HANDLER_ERRNO(errorHandler_, "failed to detect format (seek to end)");
            return;
        }

        size_t fileSize = ftell(file.get());
        if (!fileSize) {
            ERROR_HANDLER_ERRNO(errorHandler_, "failed to detect format (empty file)");
            return;
        }

        if (fseek(file.get(), 0L, SEEK_SET)) {
            ERROR_HANDLER_ERRNO(errorHandler_, "failed to detect format (seek to begin)");
            return;
        }

        uint8_t magicNumberBuffer[16];
        size_t readSize = fileSize > sizeof(magicNumberBuffer) ? sizeof(magicNumberBuffer) : fileSize;
        if (!fread(magicNumberBuffer, readSize, 1, file.get())) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to detect format (read file)");
            return;
        }

        file.reset(nullptr);

        // load
        if (isWebPFileData(magicNumberBuffer, readSize)) {
            loadWebP(path.data());
        } else if (isJpegFileData(magicNumberBuffer, readSize)) {
            loadJpeg(path.data());
        } else {
            loadFile(path.data());
        }
    }

    const std::string& ImageAsset::getError() {
        return errorHandler_.getError();
    }

    void ImageAsset::load(uint8_t* pixelData, size_t size, int32_t width, int32_t height, int32_t channelCount) {
        errorHandler_.clear();

        buffer_ = {new uint8_t[size], default_array_deleter<uint8_t>};
        memcpy(buffer_.get(), pixelData, size);

        width_ = width;
        height_ = height;
        channelCount_ = channelCount;
    }

    void ImageAsset::load(uint8_t* fileData, size_t size) {
        errorHandler_.clear();

        // load
        if (isWebPFileData(fileData, size)) {
            loadWebP(fileData, size);
        } else if (isJpegFileData(fileData, size)) {
            loadJpeg(fileData, size);
        } else {
            loadFile(fileData, size);
        }
    }

    void ImageAsset::loadFile(const std::string_view& path) {
        errorHandler_.clear();

        stbi_set_flip_vertically_on_load(true);
        buffer_ = {stbi_load(path.data(), &width_, &height_, &channelCount_, 0), stb_deleter};
        if (!buffer_) {
            errorHandler_.setError(stbi_failure_reason());
            return;
        }
    }

    void ImageAsset::loadFile(uint8_t* fileData, size_t size) {
        errorHandler_.clear();

        stbi_set_flip_vertically_on_load(true);

        buffer_ = {stbi_load_from_memory(fileData, size, &width_, &height_, &channelCount_, 0), stb_deleter};
        if (!buffer_) {
            errorHandler_.setError(stbi_failure_reason());
            return;
        }
    }

    void ImageAsset::loadWebP(uint8_t* fileData, size_t size) {
        errorHandler_.clear();

        if (!WebPGetInfo(fileData, size, &width_, &height_)) {
            errorHandler_.setError("Failed to get webp info");
            return;
        }

        channelCount_ = 4;

        buffer_ = {new uint8_t[width_ * height_ * channelCount_], default_array_deleter<uint8_t>};
        VP8StatusCode statusCode;
        WebPDecoderConfig decoderConfig;

        if (WebPInitDecoderConfig(&decoderConfig) == 0) {
            errorHandler_.setError("Failed to initialize decoder config");
            return;
        }

        decoderConfig.options.use_threads = true;
        decoderConfig.options.use_scaling = false;
        if ((statusCode = WebPGetFeatures(fileData, size, &decoderConfig.input)) != VP8_STATUS_OK) {
            errorHandler_.setError("Failed to call WebPGetFeatures: " + std::to_string(statusCode));
            return;
        }

        auto strideSize = channelCount_ * width_;

        decoderConfig.output.is_external_memory = true;
        decoderConfig.output.colorspace = MODE_RGBA;
        decoderConfig.output.u.RGBA.stride = -strideSize;
        decoderConfig.output.u.RGBA.rgba = buffer_.get() + (height_ - 1) * strideSize;
        decoderConfig.output.u.RGBA.size = height_ * strideSize;
        decoderConfig.output.width = width_;
        decoderConfig.output.height = height_;

        if ((statusCode = WebPDecode(fileData, size, &decoderConfig)) != VP8_STATUS_OK) {
            errorHandler_.setError("Failed to call WebPDecode: " + std::to_string(statusCode));
            return;
        }
    }

    void ImageAsset::loadWebP(const std::string_view& path) {
        errorHandler_.clear();

        // open
        auto file{make_c_unique<FILE, FILE_deleter>(fopen(path.data(), "rb"))};
        if (!file) {
            errorHandler_.setError(strerror(errno));
            return;
        }

        // read
        if (fseek(file.get(), 0L, SEEK_END) != 0) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load webp (seek to end)");
            return;
        }

        size_t fileSize = ftell(file.get());
        if (!fileSize) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load webp (empty file)");
            return;
        }

        if (fseek(file.get(), 0L, SEEK_SET)) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load webp (seek to begin)");
            return;
        }

        std::unique_ptr<uint8_t> rdBuf(new uint8_t[fileSize]);
        if (!fread(rdBuf.get(), fileSize, 1, file.get())) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load webp (read file)");
            return;
        }

        loadWebP(rdBuf.get(), fileSize);
    }

    void ImageAsset::loadJpeg(const std::string_view& path) {
        errorHandler_.clear();

        // open
        auto file{make_c_unique<FILE, FILE_deleter>(fopen(path.data(), "rb"))};
        if (!file) {
            errorHandler_.setError(strerror(errno));
            return;
        }

        // read
        if (fseek(file.get(), 0L, SEEK_END) != 0) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load jpeg (seek to end)");
            return;
        }

        size_t fileSize = ftell(file.get());
        if (!fileSize) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load jpeg (empty file)");
            return;
        }

        if (fseek(file.get(), 0L, SEEK_SET)) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load jpeg (seek to begin)");
            return;
        }

        std::unique_ptr<uint8_t> rdBuf(new uint8_t[fileSize]);
        if (!fread(rdBuf.get(), fileSize, 1, file.get())) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to load jpeg (read file)");
            return;
        }

        loadJpeg(rdBuf.get(), fileSize);
    }

    void ImageAsset::loadJpeg(uint8_t* fileData, size_t size) {
        errorHandler_.clear();

        c_unique_ptr<void, tj_deleter> processor{nullptr, tj_deleter};
        /*
         c_unique_ptr<uint8_t, tj_deleter> transformedData{nullptr, tj_deleter};
         uint64_t transformedSize = 0;

         // transform
         processor.reset(tjInitTransform());
         if (!processor) {
             errorHandler_.setError(tjGetErrorStr());
             return;
         }

         tjtransform transform{};
         memset(&transform, 0, sizeof(tjtransform));
         transform.op = TJXOP_VFLIP;

         uint8_t* transformedDataRaw = nullptr;
         if (tjTransform(processor.get(), fileData, size, 1, &transformedDataRaw,
                         reinterpret_cast<unsigned long*>(&transformedSize), &transform, 0) != 0) {
             errorHandler_.setError(tjGetErrorStr2(processor.get()));
             return;
         }
         transformedData.reset(transformedDataRaw);

 */
        // decode
        processor.reset(tjInitDecompress());
        if (!processor) {
            errorHandler_.setError(tjGetErrorStr());
            return;
        }

        int jpegSubsample, jpegColorspace;
        if (tjDecompressHeader3(processor.get(), fileData,
                                size, &width_, &height_, &jpegSubsample, &jpegColorspace) != 0) {
            errorHandler_.setError(tjGetErrorStr2(processor.get()));
            return;
        }

        channelCount_ = 3;

        Buffer buffer{new uint8_t[width_ * height_ * channelCount_], default_array_deleter<uint8_t>};
        if (tjDecompress2(processor.get(),
/*                          transformedData.get(),
                          transformedSize,*/
                          fileData,
                          size,
                          buffer.get(), width_,
                          0,
                          height_,
                          TJPF_RGB,
                          TJFLAG_FASTDCT | TJFLAG_BOTTOMUP) != 0) {
            errorHandler_.setError(tjGetErrorStr2(processor.get()));
            return;
        }

        buffer_ = std::move(buffer);
    }

    int32_t ImageAsset::getWidth() {
        errorHandler_.clear();

        if (!buffer_) {
            errorHandler_.setError(ERROR_STR_IMAGE_NOT_LOADED);
            return -1;
        }

        return width_;
    }

    int32_t ImageAsset::getHeight() {
        errorHandler_.clear();

        if (!buffer_) {
            errorHandler_.setError(ERROR_STR_IMAGE_NOT_LOADED);
            return -1;
        }

        return height_;
    }

    int32_t ImageAsset::getChannelCount() {
        errorHandler_.clear();

        if (!buffer_) {
            errorHandler_.setError(ERROR_STR_IMAGE_NOT_LOADED);
            return -1;
        }

        return channelCount_;
    }

    void ImageAsset::clip(int32_t x, int32_t y, int32_t width, int32_t height) {
        errorHandler_.clear();

        if (!buffer_) {
            errorHandler_.setError(ERROR_STR_IMAGE_NOT_LOADED);
            return;
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
            errorHandler_.setError(ERROR_STR_IMAGE_SIZE_OVERFLOW);
            return;
        }

        Buffer newBuffer{new uint8_t[width * height * channelCount_], default_array_deleter<uint8_t>};

        for (int iy = 0; iy < height; iy++) {
            memcpy(newBuffer.get() + newStrideSize * iy,
                   buffer_.get() + srcStrideSize * (iy + startLine) + x * channelCount_, newStrideSize);
        }

        buffer_ = std::move(newBuffer);
        width_ = width;
        height_ = height;
    }

    void ImageAsset::resize(int32_t width, int32_t height) {
        errorHandler_.clear();

        if (!buffer_) {
            errorHandler_.setError(ERROR_STR_IMAGE_NOT_LOADED);
            return;
        }

        Buffer newBuffer{new uint8_t[width * height * channelCount_], default_array_deleter<uint8_t>};

        switch (channelCount_) {
            case 3: {
                ScaleImage<3>(buffer_.get(), newBuffer.get(), width_, height_, width, height);
                break;
            }
            case 4: {
                ScaleImage<4>(buffer_.get(), newBuffer.get(), width_, height_, width, height);
                break;
            }
            default:
                break;
        }

        width_ = width;
        height_ = height;

        buffer_ = std::move(newBuffer);
    }

    void ImageAsset::unload() {
        errorHandler_.clear();

        if (!buffer_) {
            errorHandler_.setError(ERROR_STR_IMAGE_NOT_LOADED);
            return;
        }

        buffer_.reset();
        width_ = 0;
        height_ = 0;
        channelCount_ = 0;
    }

    void ImageAsset::copyTo(void* buffer) {
        errorHandler_.clear();

        if (!buffer_) {
            errorHandler_.setError(ERROR_STR_IMAGE_NOT_LOADED);
            return;
        }

        memcpy(buffer, buffer_.get(), width_ * height_ * channelCount_);
    }

    ImageAsset* ImageAsset::clone() const {
        auto result = new ImageAsset;

        result->load(buffer_.get(), width_ * height_ * channelCount_, width_, height_, channelCount_);

        return result;
    }

    void stb_deleter(uint8_t* buffer) {
        stbi_image_free(buffer);
    }
} // Uniasset