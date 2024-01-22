//
// Created by Twiiz on 2024/1/14.
//

#include "AudioPlayer.hpp"

#include <cstring>

#include "miniaudio.h"
#include "AudioAsset.hpp"
#include "IAudioDecoder.hpp"

namespace Uniasset {

    const char* ERROR_STR_AUDIO_NOT_LOADED = "audio asset is not loaded";
    const char* ERROR_STR_AUDIO_NOT_OPENED = "audio player has not opened any audio asset";

    inline ma_format ToMaFormat(SampleFormat format) {
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

    void AudioPlayer::MaDataCallback(ma_device* device, void* buffer, const void* unused1,
                                     unsigned int count) {
        (void) unused1;

        auto player = reinterpret_cast<AudioPlayer*>(device->pUserData);

        do {
            if (player->state_ == Paused) {
                break;
            }

            if (!player->audioDecoder_->Read(buffer, count)) {
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

    AudioPlayer::AudioPlayer()
            : device_{new ma_device} {
    }

    const std::string& AudioPlayer::GetError() {
        return errorHandler_.GetError();
    }

    void AudioPlayer::Open(AudioAsset* audioAsset) {
        errorHandler_.Clear();

        if (state_ != Closed) {
            Close();
        }

        // get decoder
        auto audioDecoder = audioAsset->GetAudioDecoder();

        if (!audioDecoder) {
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_LOADED);
            return;
        }

        // init device
        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = ToMaFormat(audioDecoder->GetSampleFormat());
        deviceConfig.playback.channels = audioDecoder->GetChannelCount();
        deviceConfig.sampleRate = audioDecoder->GetSampleRate();
        deviceConfig.pUserData = this;
        deviceConfig.dataCallback = &MaDataCallback;

        if (ma_device_init(nullptr, &deviceConfig, device_) != MA_SUCCESS) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to create playback device");
            return;
        }

        ma_device_set_master_volume(device_, volume_);

        // attach asset
        audioAsset->AttachPlayer(this);

        audioAsset_ = audioAsset;
        audioDecoder_ = audioDecoder.release();

        // set state
        state_ = Opened;
        channelCount_ = audioDecoder_->GetChannelCount();
        sampleRate_ = audioDecoder_->GetSampleRate();
    }

    void AudioPlayer::Close() {
        if (state_ == Closed) {
            return;
        }

        audioAsset_->DetachPlayer(this);
        CloseInternal();
    }

    void AudioPlayer::CloseInternal() {
        if (state_ == Closed) {
            return;
        }

        // reset states
        state_ = Closed;
        decodedSampleCount_ = 0;
        sampleRate_ = 0;
        channelCount_ = 0;

        // dispose device
        ma_device_uninit(device_);

        // dispose audio decoder
        delete audioDecoder_;
        audioDecoder_ = nullptr;

        // detach asset
        audioAsset_ = nullptr;
    }

    AudioPlayer::~AudioPlayer() {
        Close();

        delete device_;
    }

    void AudioPlayer::Pause() {
        if (state_ == Resumed) {
            state_ = Paused;
        }
    }

    void AudioPlayer::Resume() {
        if (state_ == Opened) {
            ma_device_start(device_);
            state_ = Paused;
        }

        if (state_ == Paused) {
            state_ = Resumed;
        }
    }

    bool AudioPlayer::IsPaused() {
        return state_ != Resumed;
    }

    float AudioPlayer::GetVolume() {
        if (state_ == Closed) {
            return volume_;
        }

        ma_device_get_master_volume(device_, &volume_);

        return volume_;
    }

    void AudioPlayer::SetVolume(float val) {
        volume_ = val;

        if (state_ == Closed) {
            return;
        }

        ma_device_set_master_volume(device_, val);
    }

    float AudioPlayer::GetTime() const {
        return static_cast<float>(decodedSampleCount_) /
               static_cast<float>(sampleRate_);
    }
} // Uniasset