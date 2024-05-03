//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_MINIAUDIOPLAYER_H
#define UNIASSET_MINIAUDIOPLAYER_H

#include <memory>
#include "uniasset/Foundation.hpp"
#include "uniasset/audio/AudioDevice.hpp"
#include "uniasset/thirdparty/miniaudio.hpp"

struct ma_device;

namespace uniasset {
    class AudioAsset;

    class IAudioDecoder;

    /*class UNIASSET_API AudioPlayer : public std::enable_shared_from_this<AudioPlayer> {
    private:
        enum State {
            Closed,
            Resumed,
            Opened,
            Paused
        };

        c_unique_ptr<ma_device, miniaudio_deleter> device_{nullptr, miniaudio_deleter};

        std::shared_ptr<IAudioDecoder> audioDecoder_{nullptr};

        State state_{Closed};
        float volume_{1};
        size_t decodedSampleCount_{0};
        uint32_t sampleRate_{0};
        uint32_t channelCount_{0};

        static void maDataCallback(ma_device* device, void* buffer, const void* unused1, unsigned int frameCount);

    public:
        explicit AudioPlayer();

        AudioPlayer(const AudioPlayer&) = delete;

        AudioPlayer& operator=(const AudioPlayer&) = delete;

        const std::string& getError();

        void open(const std::shared_ptr<AudioAsset>& audioAsset);
        
        void open(const std::shared_ptr<IAudioDecoder>& audioDecoder);

        void pause();

        void resume();

        bool isPaused();

        void close();

        float getVolume();

        void setVolume(float val);

        float getTime() const;

        void setTime(float time);

    };*/

} // Uniasset

#endif //UNIASSET_MINIAUDIOPLAYER_H
