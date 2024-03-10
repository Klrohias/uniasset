//
// Created by Twiiz on 2024/1/14.
//

#include "AudioPlayer.hpp"

#include <cstring>

#include "../common/Errors.hpp"
#include "../common/Templates.hpp"
#include "AudioAsset.hpp"
#include "IAudioDecoder.hpp"

namespace uniasset {
    inline ma_format toMaFormat(SampleFormat format) {
        switch (format) {
            case SampleFormat_Float:
                return ma_format_f32;
            case SampleFormat_Int16:
                return ma_format_s16;
        }
        return ma_format_u8;
    }

    void AudioPlayer::maDataCallback(ma_device* device, void* buffer, const void* unused1,
                                     unsigned int frameCount) {
        (void) unused1;

        auto player = reinterpret_cast<AudioPlayer*>(device->pUserData);

        auto frameSize = device->playback.channels;
        switch (device->playback.format) {
            case ma_format_s16:
                frameSize *= sizeof(int16_t);
                break;
            case ma_format_f32:
                frameSize *= sizeof(float);
                break;
            default:
                break;
        }

        if (player->state_ == Paused) {
            // paused, fill zero
            memset(buffer, 0, frameSize * frameCount);
        }

        // read frame
        if (uint32_t readFrameCount = player->audioDecoder_->read(buffer, frameCount);
                readFrameCount < frameCount) {
            memset(ptr_offset(buffer, readFrameCount * frameSize), 0, (frameCount - readFrameCount) * frameSize);
        }

        // decodedSampleCount_ is used for time calculation
        player->decodedSampleCount_ += frameCount;
    }

    AudioPlayer::AudioPlayer() = default;

    const std::string& AudioPlayer::getError() {
        return errorHandler_.getError();
    }

    void AudioPlayer::open(const std::shared_ptr<AudioAsset>& audioAsset) {
        errorHandler_.clear();

        // get decoder
        auto audioDecoder = audioAsset->getAudioDecoder(SampleFormat_Int16);

        if (!audioDecoder) {
            errorHandler_.setError(ERROR_STR_AUDIO_NOT_LOADED);
            return;
        }

        // init device
        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = toMaFormat(audioDecoder->getSampleFormat());
        deviceConfig.playback.channels = audioDecoder->getChannelCount();
        deviceConfig.sampleRate = audioDecoder->getSampleRate();
        deviceConfig.pUserData = this;
        deviceConfig.dataCallback = &maDataCallback;

        device_ = miniaudio_create_device(deviceConfig);

        if (!device_) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to create playback device");
            return;
        }

        ma_device_set_master_volume(device_.get(), volume_);

        audioAsset_ = audioAsset;
        audioDecoder_ = std::shared_ptr<IAudioDecoder>(audioDecoder.release());

        // set state
        state_ = Opened;
        decodedSampleCount_ = 0;
        channelCount_ = audioDecoder_->getChannelCount();
        sampleRate_ = audioDecoder_->getSampleRate();
    }

    void AudioPlayer::close() {
        if (state_ == Closed) {
            return;
        }

        // reset states
        state_ = Closed;
        decodedSampleCount_ = 0;
        sampleRate_ = 0;
        channelCount_ = 0;

        device_.reset();

        audioAsset_ = nullptr;
        audioDecoder_ = nullptr;
    }

    void AudioPlayer::pause() {
        if (state_ == Resumed) {
            state_ = Paused;
        }
    }

    void AudioPlayer::resume() {
        if (state_ == Opened) {
            ma_device_start(device_.get());
            state_ = Paused;
        }

        if (state_ == Paused) {
            state_ = Resumed;
        }
    }

    bool AudioPlayer::isPaused() {
        return state_ != Resumed;
    }

    float AudioPlayer::getVolume() {
        if (state_ == Closed) {
            return volume_;
        }

        ma_device_get_master_volume(device_.get(), &volume_);

        return volume_;
    }

    void AudioPlayer::setVolume(float val) {
        volume_ = val;

        if (state_ == Closed) {
            return;
        }

        ma_device_set_master_volume(device_.get(), val);
    }

    float AudioPlayer::getTime() const {
        return static_cast<float>(audioDecoder_->tell()) / static_cast<float>(sampleRate_);
    }

    void AudioPlayer::setTime(float time) {
        audioDecoder_->seek(static_cast<int64_t>(time * static_cast<float>(sampleRate_)));
    }
} // Uniasset