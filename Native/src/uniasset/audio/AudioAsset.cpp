//
// Created by Twiiz on 2024/1/14.
//

#include "AudioAsset.hpp"

#include "AudioPlayer.hpp"
#include "IAudioDecoder.hpp"
#include "../common/Utils.hpp"
#include "Mp3Decoder.hpp"
#include "FlacDecoder.hpp"
#include "WavDecoder.hpp"
#include "OggDecoder.hpp"

using namespace Uniasset::Utils;

namespace Uniasset {

    const char* ERROR_STR_AUDIO_NOT_SUPPORTED = "audio format is not supported";
    const char* ERROR_STR_AUDIO_METADATA = "failed to read audio metadata";
    extern const char* ERROR_STR_AUDIO_NOT_LOADED;

    AudioAsset::AudioAsset() = default;

    AudioAsset::~AudioAsset() {
        Cleanup();
    }

    void AudioAsset::AttachPlayer(AudioPlayer* player) {
        playingAudioPlayer_.push_back(player);
    }

    void AudioAsset::DetachPlayer(AudioPlayer* player) {
        std::remove(playingAudioPlayer_.begin(), playingAudioPlayer_.end(), player);
    }

    std::unique_ptr<IAudioDecoder> AudioAsset::GetAudioDecoder() {
        switch (loadInfo_.format) {
            case Pcm:
                break;
            case Mp3:
                return std::unique_ptr<IAudioDecoder>(
                        loadInfo_.type == File ?
                        new Mp3Decoder(loadInfo_.path) : new Mp3Decoder(loadInfo_.data, loadInfo_.dataLength)
                );
            case Ogg:
                return std::unique_ptr<IAudioDecoder>(
                        loadInfo_.type == File ?
                        new OggDecoder(loadInfo_.path) : new OggDecoder(loadInfo_.data, loadInfo_.dataLength)
                );
            case Wav:
                return std::unique_ptr<IAudioDecoder>(
                        loadInfo_.type == File ?
                        new WavDecoder(loadInfo_.path) : new WavDecoder(loadInfo_.data, loadInfo_.dataLength)
                );
            case Flac:
                return std::unique_ptr<IAudioDecoder>(
                        loadInfo_.type == File ?
                        new FlacDecoder(loadInfo_.path) : new FlacDecoder(loadInfo_.data, loadInfo_.dataLength)
                );
        }
        return nullptr;
    }

    const std::string& AudioAsset::GetError() {
        return errorHandler_.GetError();
    }

    void AudioAsset::Load(uint8_t* data, size_t len) {
        errorHandler_.Clear();

        Cleanup();

        loadInfo_.type = Memory;
        loadInfo_.data = new uint8_t[len];
        loadInfo_.dataLength = len;
        memcpy(loadInfo_.data, data, len);

        if (IsMp3FileData(data, len)) {
            loadInfo_.format = Mp3;
        } else if (IsFlacFileData(data, len)) {
            loadInfo_.format = Flac;
        } else if (IsOggFileData(data, len)) {
            loadInfo_.format = Ogg;
        } else if (IsWavFileData(data, len)) {
            loadInfo_.format = Wav;
        } else {
            Cleanup();
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_SUPPORTED);
            return;
        }

        if (!LoadMetadata()) {
            errorHandler_.SetError(ERROR_STR_AUDIO_METADATA);
            Cleanup();
        }
    }

    void AudioAsset::Cleanup() {
        while (!playingAudioPlayer_.empty()) {
            playingAudioPlayer_.back()->CloseInternal();
            playingAudioPlayer_.pop_back();
        }

        if (loadInfo_.type == Memory) {
            delete loadInfo_.data;
        }

        loadInfo_.type = None;
        loadInfo_.data = nullptr;
        loadInfo_.path.clear();
        loadInfo_.dataLength = 0;
        loadInfo_.format = Pcm;

        sampleRate_ = 0;
        sampleCount_ = 0;
        channelCount_ = 0;
    }

    void AudioAsset::Unload() {
        errorHandler_.Clear();

        Cleanup();
    }

    void AudioAsset::Load(const std::string_view& path) {
        errorHandler_.Clear();

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
        if (IsMp3FileData(buffer, readSize)) {
            loadInfo_.format = Mp3;
        } else if (IsFlacFileData(buffer, readSize)) {
            loadInfo_.format = Flac;
        } else if (IsOggFileData(buffer, readSize)) {
            loadInfo_.format = Ogg;
        } else if (IsWavFileData(buffer, readSize)) {
            loadInfo_.format = Wav;
        } else {
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_SUPPORTED);
            return;
        }

        loadInfo_.type = File;
        loadInfo_.path = path;

        if (!LoadMetadata()) {
            errorHandler_.SetError(ERROR_STR_AUDIO_METADATA);
            Cleanup();
        }
    }

    bool AudioAsset::LoadMetadata() {
        auto decoder = GetAudioDecoder();
        if (!decoder) {
            return false;
        }

        channelCount_ = decoder->GetChannelCount();
        sampleCount_ = decoder->GetSampleCount();
        sampleRate_ = decoder->GetSampleRate();

        return true;
    }

    size_t AudioAsset::GetSampleCount() {
        errorHandler_.Clear();

        if (loadInfo_.type == None) {
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_LOADED);
            return 0;
        }

        return sampleCount_;
    }

    uint32_t AudioAsset::GetSampleRate() {
        errorHandler_.Clear();

        if (loadInfo_.type == None) {
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_LOADED);
            return 0;
        }

        return sampleRate_;
    }

    uint32_t AudioAsset::GetChannelCount() {
        errorHandler_.Clear();

        if (loadInfo_.type == None) {
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_LOADED);
            return 0;
        }

        return channelCount_;
    }
} // Uniasset