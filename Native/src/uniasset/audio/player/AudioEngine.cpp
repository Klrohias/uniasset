#include "AudioEngine.hpp"

#include "uniasset/common/Errors.hpp"

namespace uniasset {
    ma_engine* AudioEngine::engine() {
        return &engine_;
    }

    float AudioEngine::volume() const {
        return volume_;
    }

    std::error_code AudioEngine::setVolume(const float volume) {
        volume_ = volume;
        const auto result = ma_engine_set_volume(&engine_, volume);
        return result == MA_SUCCESS ? err_ok() : std::error_code{result, ma_category()};
    }

    ma_uint64 AudioEngine::getTimeInPcmFrames() const {
        return ma_engine_get_time_in_pcm_frames(&engine_);
    }

    std::error_code AudioEngine::resetTimeInPcmFrames() {
        const auto result = ma_engine_set_time_in_pcm_frames(&engine_, 0);
        return result == MA_SUCCESS ? err_ok() : std::error_code{result, ma_category()};
    }

    ma_uint32 AudioEngine::getSampleRate() const {
        return ma_engine_get_sample_rate(&engine_);
    }

    Result<std::unique_ptr<PlaybackInstance>> AudioEngine::createPlayback(const std::shared_ptr<IAudioDecoder>& decoder) {
        return PlaybackInstance::create(shared_from_this(), decoder, 0);
    }

    AudioEngine::~AudioEngine() {
        uninit();
    }

    Result<std::unique_ptr<AudioEngine>> AudioEngine::create() {
        auto engine = std::make_unique<AudioEngine>();
        if (const auto err = engine->init(); err != err_ok()) {
            return err;
        }
        return engine;
    }

    std::error_code AudioEngine::init() {
        const auto engine_config = ma_engine_config_init();
        const auto result = ma_engine_init(&engine_config, &engine_);
        return result == MA_SUCCESS ? err_ok() : std::error_code{result, ma_category()};
    }

    void AudioEngine::uninit() {
        ma_engine_uninit(&engine_);
    }
}
