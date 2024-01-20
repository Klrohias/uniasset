//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_MINIAUDIOPLAYER_H
#define UNIASSET_MINIAUDIOPLAYER_H

#include <miniaudio.h>

#include <miniaudio.h>
#include "uniasset/Foundation.hpp"
#include "uniasset/common/ErrorHandler.hpp"

struct ma_device;

namespace Uniasset {
    class AudioAsset;
    class IAudioDecoder;

    class UNIASSET_API AudioPlayer {
    private:
        ma_device* device_{nullptr};
        ErrorHandler errorHandler_{};
        AudioAsset* audioAsset_{nullptr};
        IAudioDecoder* audioDecoder_{nullptr};
        bool paused_{true};

        static void MaDataCallback(ma_device* device, void* buffer, __attribute__((unused)) const void* unused, unsigned int count);

    public:
        explicit AudioPlayer();

        ~AudioPlayer();

        AudioPlayer(const AudioPlayer&) = delete;

        AudioPlayer& operator=(const AudioPlayer&) = delete;

        const std::string& GetError();

        bool Open(AudioAsset* audioAsset);

        bool Pause();

        bool Resume();

        bool IsPaused();

        bool Close();

        float GetVolume();

        bool SetVolume(float val);

    };

} // Uniasset

#endif //UNIASSET_MINIAUDIOPLAYER_H
