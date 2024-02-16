//
// Created by Twiiz on 2024/1/14.
//

#include "AudioPlayer.hpp"

#include <cstring>

#include "AudioAsset.hpp"
#include "IAudioDecoder.hpp"

namespace uniasset {

    const char ERROR_STR_AUDIO_NOT_LOADED[] = "audio asset is not loaded";
    const char ERROR_STR_AUDIO_NOT_OPENED[] = "audio player has not opened any audio asset";

    inline ma_format toMaFormat(SampleFormat format) {
        switch (format) {
            case Uint8:
                return ma_format_u8;
            case Int32:
                return ma_format_s32;
            case Float:
                return ma_format_f32;
            case Int16:
                return ma_format_s16;
        }
        return ma_format_u8;
    }

    void AudioPlayer::maDataCallback(ma_device* device, void* buffer, const void* unused1,
                                     unsigned int count) {
        (void) unused1;

        auto player = reinterpret_cast<AudioPlayer*>(device->pUserData);

        do {
            if (player->state_ == Paused) {
                break;
            }

            if (!player->audioDecoder_->read(buffer, count)) {
                break;
            }

            player->decodedSampleCount_ += count;
            return;
        } while (false);

        auto bufferLength = device->playback.channels * count;
        switch (device->playback.format) {
            case ma_format_u8:
                memset(buffer, 0, sizeof(uint8_t) * bufferLength);
                break;
            case ma_format_s16:
                memset(buffer, 0, sizeof(int16_t) * bufferLength);
                break;
            case ma_format_s32:
            case ma_format_f32:
                memset(buffer, 0, sizeof(int32_t) * bufferLength);
                break;
            default:
                break;
        }
    }

    AudioPlayer::AudioPlayer() = default;

    const std::string& AudioPlayer::getError() {
        return errorHandler_.getError();
    }

    void AudioPlayer::open(const std::shared_ptr<AudioAsset>& audioAsset) {
        errorHandler_.clear();

        // get decoder
        auto audioDecoder = audioAsset->getAudioDecoder();

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
        return static_cast<float>(decodedSampleCount_) /
               static_cast<float>(sampleRate_);
    }
} // Uniasset