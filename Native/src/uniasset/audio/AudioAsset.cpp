//
// Created by Twiiz on 2024/1/14.
//

#include "AudioAsset.hpp"

#include "AudioPlayer.hpp"
#include "IAudioDecoder.hpp"
#include "../common/Utils.hpp"

using namespace Uniasset::Utils;

namespace Uniasset {

    const char* ERROR_STR_AUDIO_NOT_SUPPORTED = "audio format is not supported";

    AudioAsset::AudioAsset() = default;

    AudioAsset::~AudioAsset() {
        disposing_ = true;
        for (const auto& player: playingAudioPlayer_) {
            player->Close();
        }
    }

    void AudioAsset::AttachPlayer(AudioPlayer* player) {
        playingAudioPlayer_.push_back(player);
    }

    void AudioAsset::DetachPlayer(AudioPlayer* player) {
        if (disposing_) return;
        std::remove(playingAudioPlayer_.begin(), playingAudioPlayer_.end(), player);
    }

    std::unique_ptr<IAudioDecoder> AudioAsset::GetAudioDecoder() {
        return nullptr;
    }

    const std::string& AudioAsset::GetError() {
        return errorHandler_.GetError();
    }

    bool AudioAsset::Load(uint8_t* data, size_t len) {
        // TODO
        if(!playingAudioPlayer_.empty()) {

        }

        if (!IsFlacFileData(data, len)
            && !IsMp3FileData(data, len)
            && !IsOggFileData(data, len)
            && !IsWavFileData(data, len)) {
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_SUPPORTED);
            return false;
        }

        CleanupLoadInfo();
        loadInfo_.type = Memory;
        loadInfo_.data = new uint8_t [len];
        memcpy(loadInfo_.data, data, len);

        return true;
    }

    void AudioAsset::CleanupLoadInfo() {
        if (loadInfo_.type == Memory) {
            delete loadInfo_.data;
        }

        loadInfo_.type = None;
        loadInfo_.data = nullptr;
        loadInfo_.path.clear();
    }
} // Uniasset