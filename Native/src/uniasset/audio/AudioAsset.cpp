//
// Created by Twiiz on 2024/1/14.
//

#include "AudioAsset.hpp"

#include <cstring>
#include <fstream>
#include <memory>

#include "../common/Errors.hpp"
#include "../common/MagicNumbers.hpp"
#include "IAudioDecoder.hpp"
#include "Mp3Decoder.hpp"
#include "FlacDecoder.hpp"
#include "WavDecoder.hpp"
#include "OggDecoder.hpp"
#include "BufferedAudioDecoder.hpp"

namespace uniasset {
    static bool tryIdentifyFormat(uint8_t* ptr, size_t length, DataFormat& outFormat) {
        if (isMp3FileData(ptr, length)) {
            outFormat = DataFormat_Mp3;
        } else if (isFlacFileData(ptr, length)) {
            outFormat = DataFormat_Flac;
        } else if (isOggFileData(ptr, length)) {
            outFormat = DataFormat_Ogg;
        } else if (isWavFileData(ptr, length)) {
            outFormat = DataFormat_Wav;
        } else {
            outFormat = DataFormat_Pcm;
            return false;
        }
        return true;
    }

    Result<std::unique_ptr<IAudioDecoder>> AudioAsset::getAudioDecoder(SampleFormat sampleFormat, int64_t frameBufferSize) {
        if (type_ == LoadType_None) {
            return std::error_code{kAudioNotLoadFail, uniasset_category()};
        }

        IAudioDecoder* rawDecoder{nullptr};

        switch (format_) {
            case DataFormat_Pcm:
                return std::error_code{kNotSupportedFail, uniasset_category()};
            case DataFormat_Mp3:
                rawDecoder = new Mp3Decoder{shared_from_this(), sampleFormat};
                break;
            case DataFormat_Ogg:
                rawDecoder = new OggDecoder{shared_from_this(), sampleFormat};
                break;
            case DataFormat_Wav:
                rawDecoder = new WavDecoder{shared_from_this(), sampleFormat};
                break;
            case DataFormat_Flac:
                rawDecoder = new FlacDecoder{shared_from_this(), sampleFormat};
                break;
        }

        std::shared_ptr<IAudioDecoder> sharedRawDecoder{rawDecoder};
        return std::unique_ptr<IAudioDecoder>(new BufferedAudioDecoder{sharedRawDecoder, frameBufferSize});
    }

    std::error_code AudioAsset::load(Buffer&& data, size_t size) {
        if (!tryIdentifyFormat(data.get(), size, format_)) {
            return {kNotSupportedFail, uniasset_category()};
        }

        type_ = LoadType_Memory;
        data_ = std::move(data);
        dataLength_ = size;

        if (const auto err = loadMetadata(); err.value()) {
            return err;
        }

        return err_ok();
    }

    std::error_code AudioAsset::load(const std::span<uint8_t>& data) {
        auto content = data.data();
        auto len = data.size();

        if (!tryIdentifyFormat(content, len, format_)) {
            return {kNotSupportedFail, uniasset_category()};
        }

        type_ = LoadType_Memory;
        data_ = {new uint8_t[len], default_array_deleter<uint8_t>};
        dataLength_ = len;
        memcpy(data_.get(), content, len);

        if (auto err = loadMetadata(); err.value()) {
            return err;
        }

        return err_ok();
    }

    void AudioAsset::unload() {
        type_ = LoadType_None;
        path_.clear();
        data_.reset();
    }

    std::error_code AudioAsset::load(const std::filesystem::path& path) {
        // Check if the file exists and is a regular file
        if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
            return err_errno(); // Or a more specific error
        }

        // Use std::ifstream for C++ stream-based file reading
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return err_errno();
        }

        // Read the first 32 bytes
        std::vector<uint8_t> buffer(32);
        file.read(reinterpret_cast<char*>(buffer.data()), static_cast<int64_t>(buffer.size()));

        // Check for read errors and if any bytes were read
        if (!file.good() && !file.eof()) {
            return err_errno();
        }

        const size_t readSize = file.gcount();
        if (readSize == 0) {
            return err_errno();
        }

        // Check magic number
        if (!tryIdentifyFormat(buffer.data(), readSize, format_)) {
            return {kNotSupportedFail, uniasset_category()};
        }

        type_ = LoadType_File;
        path_ = path;

        if (auto err = loadMetadata(); err.value()) {
            return err;
        }

        return err_ok();
    }

    std::error_code AudioAsset::loadMetadata() {
        auto decoderResult = getAudioDecoder(SampleFormat_Int16);
        if (auto err = decoderResult.error(); err.has_value()) {
            return *err.value();
        }

        auto& decoder = **decoderResult.data();

        channelCount_ = decoder->getChannelCount();
        sampleCount_ = decoder->getSampleCount();
        sampleRate_ = decoder->getSampleRate();

        return err_ok();
    }

    Result<size_t> AudioAsset::getSampleCount() {
        if (type_ == LoadType_None) {
            return std::error_code{kAudioNotLoadFail, uniasset_category()};
        }

        return sampleCount_;
    }

    Result<uint32_t> AudioAsset::getSampleRate() {
        if (type_ == LoadType_None) {
            return std::error_code{kAudioNotLoadFail, uniasset_category()};
        }

        return sampleRate_;
    }

    Result<uint32_t> AudioAsset::getChannelCount() {
        if (type_ == LoadType_None) {
            return std::error_code{kAudioNotLoadFail, uniasset_category()};
        }

        return channelCount_;
    }

    Result<float> AudioAsset::getLength() {
        if (type_ == LoadType_None) {
            return std::error_code{kAudioNotLoadFail, uniasset_category()};
        }

        return static_cast<float>(sampleCount_) / static_cast<float>(channelCount_) / static_cast<float>(sampleRate_);
    }

    LoadType AudioAsset::getLoadType() {
        return type_;
    }

    Result<const std::filesystem::path&> AudioAsset::getPath() const {
        if (type_ == LoadType_None) {
            return std::error_code{kAudioNotLoadFail, uniasset_category()};
        }

        return path_;
    }

    const Buffer& AudioAsset::getData() {
        return data_;
    }

    size_t AudioAsset::getDataLength() const {
        return dataLength_;
    }

    AudioAsset::AudioAsset() = default;
} // Uniasset