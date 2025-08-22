#include "AudioEngine.hpp"

#include "uniasset/common/Errors.hpp"

namespace uniasset
{
    Result<ma_engine*> AudioEngine::engine()
    {
        if (!is_init_)
        {
            return std::error_code{MA_INVALID_OPERATION, ma_category()};
        }
        return &engine_;
    }

    Result<float> AudioEngine::volume() const
    {
        return volume_;
    }

    std::error_code AudioEngine::setVolume(const float volume)
    {
        if (is_init_)
        {
            const auto result = ma_engine_set_volume(&engine_, volume);
            if (result != MA_SUCCESS)
            {
                return std::error_code{result, ma_category()};
            }
        }
        return err_ok();
    }

    ma_uint64 AudioEngine::getTimeInPcmFrames() const
    {
        return ma_engine_get_time_in_pcm_frames(&engine_);
    }

    std::error_code AudioEngine::resetTimeInPcmFrames()
    {
        const auto result = ma_engine_set_time_in_pcm_frames(&engine_, 0);
        return result == MA_SUCCESS ? err_ok() : std::error_code{result, ma_category()};
    }

    Result<std::unique_ptr<PlaybackInstance>> AudioEngine::createPlayback(std::unique_ptr<IAudioDecoder>&& decoder)
    {
        return PlaybackInstance::create(shared_from_this(), std::move(decoder), 0);
    }

    AudioEngine::AudioEngine()
    {
        engine_config_ = ma_engine_config_init();
    }

    AudioEngine::~AudioEngine()
    {
        uninit();
    }

    std::unique_ptr<AudioEngine> AudioEngine::create() {
        return std::make_unique<AudioEngine>();
    }

    std::error_code AudioEngine::init()
    {
        if (is_init_)
        {
            return std::error_code{MA_INVALID_OPERATION, ma_category()};
        }

        const auto result = ma_engine_init(&engine_config_, &engine_);
        if (result != MA_SUCCESS)
        {
            return { result, ma_category() };
        }

        is_init_ = true;
        return err_ok();
    }

    void AudioEngine::uninit()
    {
        if (!is_init_)
        {
            return;
        }

        ma_engine_uninit(&engine_);
    }
}
