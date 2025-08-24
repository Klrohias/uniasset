#include "PlaybackInstance.hpp"

#include "AudioEngine.hpp"
#include "uniasset/common/Errors.hpp"

namespace uniasset {
    Result<std::unique_ptr<PlaybackInstance>> PlaybackInstance::create(const std::shared_ptr<AudioEngine>& engine, const std::shared_ptr<IAudioDecoder>& decoder, const uint32_t flags) {
        const auto playback = new PlaybackInstance(engine, decoder);
        const auto result = playback->init(flags);
        if (result != err_ok()) {
            delete playback;
            return result;
        }
        return std::unique_ptr<PlaybackInstance>(playback);
    }

    Result<std::unique_ptr<PlaybackInstance>> PlaybackInstance::create(const std::shared_ptr<AudioEngine>& engine, const std::shared_ptr<DecoderDataSource>& source, const uint32_t flags) {
        const auto playback = new PlaybackInstance(engine, source);
        const auto result = playback->init(flags);
        if (result != err_ok()) {
            delete playback;
            return result;
        }
        return std::unique_ptr<PlaybackInstance>(playback);
    }

    PlaybackInstance::~PlaybackInstance() {
        uninit();
    }

    std::error_code PlaybackInstance::init(const uint32_t flags) {
        const auto result = ma_sound_init_from_data_source(**engine_->engine().data(), source_.get(), flags, nullptr, &sound_);
        return result == MA_SUCCESS ? err_ok() : std::error_code{result, ma_category()};
    }

    void PlaybackInstance::uninit() {
        ma_sound_uninit(&sound_);
    }

    Result<float> PlaybackInstance::volume() const {
        return ma_sound_get_volume(&sound_);
    }

    void PlaybackInstance::setVolume(const float volume) {
        ma_sound_set_volume(&sound_, volume);
    }

    Result<float> PlaybackInstance::time() const {
        return static_cast<float>(ma_sound_get_time_in_milliseconds(&sound_)) / 1000.0f;
    }

    std::error_code PlaybackInstance::setTime(const float time) {
        const auto pcm_frame = static_cast<ma_uint64>(time * static_cast<float>(source_->sampleRate()));
        const auto result = ma_sound_seek_to_pcm_frame(&sound_, pcm_frame);
        if (result != MA_SUCCESS)
            return std::error_code{result, ma_category()};
        return err_ok();
    }

    std::error_code PlaybackInstance::play() {
        ma_sound_set_stop_time_in_pcm_frames(&sound_, std::numeric_limits<uint64_t>::max());
        const auto result = ma_sound_start(&sound_);
        if (result != MA_SUCCESS)
            return std::error_code{result, ma_category()};
        return err_ok();
    }

    std::error_code PlaybackInstance::stop() {
        const auto result = ma_sound_stop(&sound_);
        if (result != MA_SUCCESS)
            return std::error_code{result, ma_category()};
        return err_ok();
    }

    bool PlaybackInstance::isPlaying() const {
        return ma_sound_is_playing(&sound_);
    }

    std::error_code PlaybackInstance::playScheduled(const ma_uint64 frame) {
        ma_sound_set_stop_time_in_pcm_frames(&sound_, std::numeric_limits<uint64_t>::max());
        ma_sound_set_start_time_in_pcm_frames(&sound_, frame);
        auto result = ma_sound_start(&sound_);
        if (result != MA_SUCCESS)
            return std::error_code{result, ma_category()};
        return err_ok();
    }

    void PlaybackInstance::stopScheduled(const ma_uint64 frame) {
        ma_sound_set_stop_time_in_pcm_frames(&sound_, frame);
        // https://github.com/mackron/miniaudio/issues/440
        // Note that the same does not apply for stopping - scheduling a stop without an explicit ma_sound_stop() should work fine.
        // ma_sound_stop(&sound_);
    }

    PlaybackInstance::PlaybackInstance(const std::shared_ptr<AudioEngine>& engine, const std::shared_ptr<IAudioDecoder>& decoder) : engine_(engine), source_(std::move(**DecoderDataSource::create(decoder).data())) {
    }

    PlaybackInstance::PlaybackInstance(const std::shared_ptr<AudioEngine>& engine, const std::shared_ptr<DecoderDataSource>& source) : engine_(engine), source_(source) {
    }
} // uniasset
