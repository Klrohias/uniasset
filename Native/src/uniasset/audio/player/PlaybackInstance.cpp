#include "PlaybackInstance.h"

#include "AudioEngine.hpp"
#include "uniasset/common/Errors.hpp"

namespace uniasset
{
    Result<std::unique_ptr<PlaybackInstance>> PlaybackInstance::create(const std::shared_ptr<AudioEngine>& engine, std::unique_ptr<IAudioDecoder>&& decoder, const uint32_t flags)
    {
        const auto playback = make_unique<PlaybackInstance>(engine, std::move(decoder));
        const auto result = playback->init(flags);
        return result != err_ok() ? result : playback;
    }

    Result<std::unique_ptr<PlaybackInstance>> PlaybackInstance::create(const std::shared_ptr<AudioEngine>& engine, std::unique_ptr<DecoderDataSource>&& source, const uint32_t flags)
    {
        const auto playback = make_unique<PlaybackInstance>(engine, std::move(source));
        const auto result = playback->init(flags);
        return result != err_ok() ? result : playback;
    }

    std::error_code PlaybackInstance::init(const uint32_t flags)
    {
        const auto result = ma_sound_init_from_data_source(**engine_->engine().data(), source_.get(), flags, nullptr, &sound_);
        return result == MA_SUCCESS ? err_ok() : std::error_code{result, ma_category()};
    }

    void PlaybackInstance::uninit()
    {
        ma_sound_uninit(&sound_);
    }

    Result<float> PlaybackInstance::volume() const
    {
        return ma_sound_get_volume(&sound_);
    }

    void PlaybackInstance::setVolume(const float volume)
    {
        ma_sound_set_volume(&sound_, volume);
    }

    Result<float> PlaybackInstance::time() const
    {
        return static_cast<float>(ma_sound_get_time_in_milliseconds(&sound_)) / 1000.0f;
    }

    std::error_code PlaybackInstance::setTime(const float time)
    {
        const auto pcm_frame = static_cast<ma_uint64>(time * static_cast<float>(source_->sampleRate()));
        const auto result = ma_sound_seek_to_pcm_frame(&sound_, pcm_frame);
        if (result != MA_SUCCESS)
            return std::error_code{result, ma_category()};
        return err_ok();
    }

    std::error_code PlaybackInstance::play()
    {
        const auto result = ma_sound_start(&sound_);
        if (result != MA_SUCCESS)
            return std::error_code{result, ma_category()};
        return err_ok();
    }

    std::error_code PlaybackInstance::stop()
    {
        const auto result = ma_sound_stop(&sound_);
        if (result != MA_SUCCESS)
            return std::error_code{result, ma_category()};
        return err_ok();
    }

    bool PlaybackInstance::isPlaying() const
    {
        return ma_sound_is_playing(&sound_);
    }

    void PlaybackInstance::playScheduled(const ma_uint64 frame)
    {
        ma_sound_set_start_time_in_pcm_frames(&sound_, frame);
    }

    void PlaybackInstance::stopScheduled(const ma_uint64 frame)
    {
        ma_sound_set_stop_time_in_pcm_frames(&sound_, frame);
    }

    PlaybackInstance::PlaybackInstance(const std::shared_ptr<AudioEngine>& engine, std::unique_ptr<IAudioDecoder>&& decoder) : engine_(engine), source_(std::move(**DecoderDataSource::create(std::move(decoder)).data()))
    {
    }

    PlaybackInstance::PlaybackInstance(const std::shared_ptr<AudioEngine>& engine, std::unique_ptr<DecoderDataSource>&& source) : engine_(engine), source_(std::move(source))
    {
    }
} // uniasset