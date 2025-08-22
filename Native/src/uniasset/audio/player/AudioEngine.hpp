#pragma once
#ifndef UNIASSET_AUDIOENGINE_H
#define UNIASSET_AUDIOENGINE_H

#include <memory>
#include <miniaudio.h>
#include <system_error>

#include "PlaybackInstance.h"
#include "uniasset/Foundation.hpp"
#include "uniasset/audio/IAudioDecoder.hpp"
#include "uniasset/common/Result.hpp"

namespace uniasset
{
    class UNIASSET_API AudioEngine : public std::enable_shared_from_this<AudioEngine>
    {
    public:
        AudioEngine(const AudioEngine&) = delete;
        AudioEngine& operator=(const AudioEngine&) = delete;

        static std::unique_ptr<AudioEngine> create();

        std::error_code init();

        void uninit();

        bool isInit() const { return is_init_; }

        ma_engine_config* config() { return &engine_config_; }

        Result<ma_engine*> engine();

        Result<float> volume() const;

        std::error_code setVolume(float volume);

        ma_uint64 getTimeInPcmFrames() const;

        std::error_code resetTimeInPcmFrames();

        Result<std::unique_ptr<PlaybackInstance>> createPlayback(std::unique_ptr<IAudioDecoder>&& decoder);

    private:
        AudioEngine();

        ~AudioEngine();

        float volume_ {1.0f};
        bool is_init_ {false};
        ma_engine engine_ {};
        ma_engine_config engine_config_ {};
    };
}

#endif // UNIASSET_AUDIOENGINE_H