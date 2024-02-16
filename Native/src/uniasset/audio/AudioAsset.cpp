//
// Created by Twiiz on 2024/1/14.
//

#include "AudioAsset.hpp"

#include <cstring>

#include "../common/Errors.hpp"
#include "../common/MagicNumbers.hpp"
#include "AudioPlayer.hpp"
#include "IAudioDecoder.hpp"
#include "Mp3Decoder.hpp"
#include "FlacDecoder.hpp"
#include "WavDecoder.hpp"
#include "OggDecoder.hpp"

using namespace uniasset::utils;

namespace uniasset {
    AudioAsset::AudioAsset() = default;


    std::unique_ptr<IAudioDecoder> AudioAsset::getAudioDecoder() {
        switch (format_) {
            case DataFormat_Pcm:
                break;
            case DataFormat_Mp3:
                return std::unique_ptr<IAudioDecoder>(new Mp3Decoder{shared_from_this()});
            case DataFormat_Ogg:
                return std::unique_ptr<IAudioDecoder>(new OggDecoder{shared_from_this()});
            case DataFormat_Wav:
                return std::unique_ptr<IAudioDecoder>(new WavDecoder{shared_from_this()});
            case DataFormat_Flac:
                return std::unique_ptr<IAudioDecoder>(new FlacDecoder{shared_from_this()});
        }
        return nullptr;
    }

    const std::string& AudioAsset::getError() {
        return errorHandler_.getError();
    }

    void AudioAsset::load(const std::span<uint8_t>& data) {
        errorHandler_.clear();

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
            errorHandler_.setError(ERROR_STR_AUDIO_NOT_SUPPORTED);
            return;
        }

        type_ = LoadType_Memory;
        data_ = {new uint8_t[len], default_array_deleter<uint8_t>};
        dataLength_ = len;
        memcpy(data_.get(), data.data(), len);

        if (!loadMetadata()) {
            errorHandler_.setError(ERROR_STR_AUDIO_METADATA);
        }
    }

    void AudioAsset::unload() {
        errorHandler_.clear();
    }

    void AudioAsset::load(const std::string_view& path) {
        errorHandler_.clear();

        // Read
        FILE* file = fopen(path.data(), "rb");

        if (!file) {
            ERROR_HANDLER_ERRNO(errorHandler_, "failed to open audio file");
            return;
        }

        uint8_t buffer[32] = {0};
        size_t readSize = fread(buffer, 32, 1, file);

        if (readSize == 0) {
            ERROR_HANDLER_ERRNO(errorHandler_, "failed to read audio file");
            return;
        }

        if (fclose(file) != 0) {
            ERROR_HANDLER_ERRNO(errorHandler_, "failed to close audio file");
            return;
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
            errorHandler_.setError(ERROR_STR_AUDIO_NOT_SUPPORTED);
            return;
        }

        type_ = LoadType_File;
        path_ = path;

        if (!loadMetadata()) {
            errorHandler_.setError(ERROR_STR_AUDIO_METADATA);
        }
    }

    bool AudioAsset::loadMetadata() {
        auto decoder = getAudioDecoder();
        if (!decoder) {
            return false;
        }

        channelCount_ = decoder->getChannelCount();
        sampleCount_ = decoder->getSampleCount();
        sampleRate_ = decoder->getSampleRate();

        return true;
    }

    size_t AudioAsset::getSampleCount() {
        errorHandler_.clear();

        if (type_ == LoadType_None) {
            errorHandler_.setError(ERROR_STR_AUDIO_NOT_LOADED);
            return 0;
        }

        return sampleCount_;
    }

    uint32_t AudioAsset::getSampleRate() {
        errorHandler_.clear();

        if (type_ == LoadType_None) {
            errorHandler_.setError(ERROR_STR_AUDIO_NOT_LOADED);
            return 0;
        }

        return sampleRate_;
    }

    uint32_t AudioAsset::getChannelCount() {
        errorHandler_.clear();

        if (type_ == LoadType_None) {
            errorHandler_.setError(ERROR_STR_AUDIO_NOT_LOADED);
            return 0;
        }

        return channelCount_;
    }

    float AudioAsset::getLength() {
        errorHandler_.clear();

        if (type_ == LoadType_None) {
            errorHandler_.setError(ERROR_STR_AUDIO_NOT_LOADED);
            return 0;
        }

        return static_cast<float>(sampleCount_) / static_cast<float>(channelCount_) / static_cast<float>(sampleRate_);
    }

    LoadType AudioAsset::getLoadType() {
        errorHandler_.clear();
        return type_;
    }

    const std::string& AudioAsset::getPath() {
        errorHandler_.clear();

        if (type_ == LoadType_None) {
            errorHandler_.setError(ERROR_STR_AUDIO_NOT_LOADED);
        }

        return path_;
    }

    const Buffer& AudioAsset::getData() {
        errorHandler_.clear();

        if (type_ == LoadType_None) {
            errorHandler_.setError(ERROR_STR_AUDIO_NOT_LOADED);
        }

        return data_;
    }

    size_t AudioAsset::getDataLength() {
        errorHandler_.clear();

        if (type_ == LoadType_None) {
            errorHandler_.setError(ERROR_STR_AUDIO_NOT_LOADED);
            return 0;
        }

        return dataLength_;
    }
} // Uniasset