//
// Created by dogdie233 on 2024/8/28.
//

#pragma once
#ifndef UNIASSET_AUDIOPLAYER_H
#define UNIASSET_AUDIOPLAYER_H

#include <memory>
#include <vector>
#include "uniasset/Foundation.hpp"
#include "uniasset/common/Result.hpp"
#include "uniasset/thirdparty/miniaudio.hpp"
#include "AudioMixer.hpp"
#include "PlayingHandler.hpp"

struct ma_device;

namespace uniasset {
    class AudioAsset;

    class IAudioDecoder;

    class UNIASSET_API AudioPlayer : public std::enable_shared_from_this<AudioPlayer> {
    
    private:
        enum State {
            Closed,
            Playing,
            Paused
        };

        struct PlayingAudio {
            std::shared_ptr<PlayingHandler> handler;
			std::shared_ptr<IWaveProvider> waveProvider;
        };

        c_unique_ptr<ma_device, miniaudio_deleter> device_{ nullptr, miniaudio_deleter };

        State state_{ Closed };
        float volume_{ 1 };
        uint64_t passedFrameCount_{ 0 };

        std::vector<PlayingAudio> playingAudios_;
        AudioMixer mixer_;

        AudioPlayer();

        ~AudioPlayer();

        static void maDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, uint32_t frameCount);

    public:
        AudioPlayer& operator=(const AudioPlayer&) = delete;

        std::error_code reset();

        void open(const std::shared_ptr<AudioAsset>& audioAsset);

        void open(const std::shared_ptr<IAudioDecoder>& audioDecoder);

        Result<std::shared_ptr<PlayingHandler>> play(const std::shared_ptr<IWaveProvider>& waveProvider);

        std::error_code pause();

        std::error_code resume();

        void close();

        Result<State> getState();

        Result<float> getVolume();
        
        std::error_code setVolume(float val);

        Result<double> getPassedTime();
    };

} // Uniasset

#endif //UNIASSET_MINIAUDIOPLAYER_H
