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

    bool AudioAsset::Load(uint8_t* data, size_t len) {
        Cleanup();

        loadInfo_.type = Memory;
        loadInfo_.data = new uint8_t[len];
        loadInfo_.dataLength = len;
        memcpy(loadInfo_.data, data, len);

        if (IsMp3FileData(data, len)) {
            loadInfo_.format = Mp3;
        }

        if (IsFlacFileData(data, len)) {
            loadInfo_.format = Flac;
        }

        if (IsOggFileData(data, len)) {
            loadInfo_.format = Ogg;
        }

        if (IsWavFileData(data, len)) {
            loadInfo_.format = Wav;
        }

        if (loadInfo_.format == Pcm) {
            Cleanup();
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_SUPPORTED);
            return false;
        }

        return true;
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
    }

    void AudioAsset::Unload() {
        Cleanup();
    }

    bool AudioAsset::Load(const std::string_view& path) {
        if (FILE *file = fopen(path.data(), "r")) {
            fclose(file);

            loadInfo_.type = File;
            loadInfo_.path = path;

            return true;
        } else {
            return false;
        }
    }
} // Uniasset