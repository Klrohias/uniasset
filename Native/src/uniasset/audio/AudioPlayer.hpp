//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_MINIAUDIOPLAYER_H
#define UNIASSET_MINIAUDIOPLAYER_H

#include <miniaudio.h>
#include <memory>
#include "uniasset/Foundation.hpp"
#include "uniasset/utils/ErrorHandler.hpp"

struct ma_device;

namespace uniasset {
    using namespace uniasset::utils;

    class AudioAsset;

    class IAudioDecoder;

    class UNIASSET_API AudioPlayer : public std::enable_shared_from_this<AudioPlayer> {
    private:
        enum State {
            Closed,
            Resumed,
            Opened,
            Paused
        };
        ma_device* device_{nullptr};
        ErrorHandler errorHandler_{};

        AudioAsset* audioAsset_{nullptr};
        IAudioDecoder* audioDecoder_{nullptr};

        State state_{Closed};
        float volume_{1};
        size_t decodedSampleCount_{0};
        uint32_t sampleRate_{0};
        uint32_t channelCount_{0};

        static void MaDataCallback(ma_device* device, void* buffer, const void* unused1, unsigned int count);

    public:
        explicit AudioPlayer();

        ~AudioPlayer();

        AudioPlayer(const AudioPlayer&) = delete;

        AudioPlayer& operator=(const AudioPlayer&) = delete;

        const std::string& GetError();

        void Open(AudioAsset* audioAsset);

        void Pause();

        void Resume();

        bool IsPaused();

        void Close();

        void CloseInternal();

        float GetVolume();

        void SetVolume(float val);

        float GetTime() const;

    };

} // Uniasset

#endif //UNIASSET_MINIAUDIOPLAYER_H
