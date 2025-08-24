#pragma once
#ifndef UNIASSET_AUDIOENGINE_H
#define UNIASSET_AUDIOENGINE_H

#include <memory>
#include <miniaudio.h>
#include <system_error>

#include "PlaybackInstance.hpp"
#include "uniasset/Foundation.hpp"
#include "uniasset/audio/IAudioDecoder.hpp"
#include "uniasset/common/Result.hpp"

namespace uniasset {
    class UNIASSET_API AudioEngine : public std::enable_shared_from_this<AudioEngine> {
    public:
        AudioEngine(const AudioEngine&) = delete;
        AudioEngine& operator=(const AudioEngine&) = delete;

        AudioEngine() = default;

        ~AudioEngine();

        static Result<std::unique_ptr<AudioEngine>> create();

        std::error_code init();

        void uninit();

        Result<ma_engine*> engine();

        float volume() const;

        std::error_code setVolume(float volume);

        ma_uint64 getTimeInPcmFrames() const;

        std::error_code resetTimeInPcmFrames();

        ma_uint32 getSampleRate() const;

        Result<std::unique_ptr<PlaybackInstance>> createPlayback(const std::shared_ptr<IAudioDecoder>& decoder);

    private:
        float volume_ {1.0f};

        ma_engine engine_ {};
    };
}

#endif // UNIASSET_AUDIOENGINE_H