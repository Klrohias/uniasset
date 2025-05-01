//
// Created by Twiiz on 2024/1/14.
//

#include "AudioAsset.hpp"

#include <cstring>

#include "../common/Errors.hpp"
#include "../common/MagicNumbers.hpp"
#include "IAudioDecoder.hpp"
#include "Mp3Decoder.hpp"
#include "FlacDecoder.hpp"
#include "WavDecoder.hpp"
#include "OggDecoder.hpp"
#include "BufferedAudioDecoder.hpp"

namespace uniasset {
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

    std::error_code AudioAsset::load(const std::span<uint8_t>& data) {
        auto content = data.data();
        auto len = data.size();

        if (isMp3FileData(data.data(), len)) {
            format_ = DataFormat_Mp3;
        } else if (isFlacFileData(data.data(), len)) {
            format_ = DataFormat_Flac;
        } else if (isOggFileData(data.data(), len)) {
            format_ = DataFormat_Ogg;
        } else if (isWavFileData(data.data(), len)) {
            format_ = DataFormat_Wav;
        } else {
            return {kNotSupportedFail, uniasset_category()};
        }

        type_ = LoadType_Memory;
        data_ = {new uint8_t[len], default_array_deleter<uint8_t>};
        dataLength_ = len;
        memcpy(data_.get(), data.data(), len);

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

    std::error_code AudioAsset::load(const std::string_view& path) {
        // Read
        FILE* file = fopen(path.data(), "rb");

        if (!file) {
            return err_errno();
        }

        uint8_t buffer[32] = {0};
        size_t readSize = fread(buffer, 1, 32, file);

        if (readSize == 0) {
            return err_errno();
        }

        if (fclose(file) != 0) {
            return err_errno();
        }

        // Check magic number
        if (isMp3FileData(buffer, readSize)) {
            format_ = DataFormat_Mp3;
        } else if (isFlacFileData(buffer, readSize)) {
            format_ = DataFormat_Flac;
        } else if (isOggFileData(buffer, readSize)) {
            format_ = DataFormat_Ogg;
        } else if (isWavFileData(buffer, readSize)) {
            format_ = DataFormat_Wav;
        } else {
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

    Result<const std::string_view> AudioAsset::getPath() {
        if (type_ == LoadType_None) {
            return std::error_code{kAudioNotLoadFail, uniasset_category()};
        }

        return std::string_view{path_};
    }

    const Buffer& AudioAsset::getData() {
        return data_;
    }

    size_t AudioAsset::getDataLength() const {
        return dataLength_;
    }

    AudioAsset::AudioAsset() = default;
} // Uniasset