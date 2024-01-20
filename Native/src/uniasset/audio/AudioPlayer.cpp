//
// Created by Twiiz on 2024/1/14.
//

#include "AudioPlayer.hpp"

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

    void AudioPlayer::MaDataCallback(ma_device* device, void* buffer, __attribute__((unused)) const void* unused1, unsigned int count) {
        auto player = reinterpret_cast<AudioPlayer*>(device->pUserData);
        if (player->paused_) {
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

            return;
        }

        player->audioDecoder_->Read(buffer, count);
    }

    AudioPlayer::AudioPlayer()
            : device_{new ma_device} {
    }

    const std::string& AudioPlayer::GetError() {
        return errorHandler_.GetError();
    }

    bool AudioPlayer::Open(AudioAsset* audioAsset) {
        if (audioAsset_) {
            Close();
        }

        // get decoder
        auto audioDecoder = audioAsset_->GetAudioDecoder();

        if (!audioDecoder_) {
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_LOADED);
            return false;
        }

        // init device
        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = ToMaFormat(audioDecoder_->GetSampleFormat());
        deviceConfig.playback.channels = audioDecoder_->GetChannelCount();
        deviceConfig.sampleRate = audioDecoder_->GetSampleRate();
        deviceConfig.pUserData = audioDecoder_;
        deviceConfig.dataCallback = &MaDataCallback;

        if (ma_device_init(nullptr, &deviceConfig, device_) != MA_SUCCESS) {
            ERROR_HANDLER_ERRNO(errorHandler_, "Failed to create playback device");
            return false;
        }

        ma_device_start(device_);

        // attach asset
        audioAsset->AttachPlayer(this);

        audioAsset_ = audioAsset;
        audioDecoder_ = audioDecoder.release();
        paused_ = true;

        return true;
    }

    bool AudioPlayer::Close() {
        if (!audioAsset_) {
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_OPENED);
            return false;
        }

        paused_ = true;

        // dispose device
        ma_device_uninit(device_);

        // detach asset
        audioAsset_->DetachPlayer(this);
        audioAsset_ = nullptr;

        // dispose audio decoder
        delete audioDecoder_;
        audioDecoder_ = nullptr;

        return true;
    }

    AudioPlayer::~AudioPlayer() {
        Close();

        delete device_;
    }

    bool AudioPlayer::Pause() {
        if (!audioAsset_) {
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_OPENED);
            return false;
        }

        if (paused_) return true;

        paused_ = true;

        return true;
    }

    bool AudioPlayer::Resume() {
        if (!audioAsset_) {
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_OPENED);
            return false;
        }

        if (!paused_) return true;

        paused_ = false;

        return true;
    }

    bool AudioPlayer::IsPaused() {
        if (!audioAsset_) {
            return true;
        }

        return paused_;
    }

    float AudioPlayer::GetVolume() {
        if (!audioAsset_) {
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_OPENED);
            return -1;
        }

        float result {1};
        ma_device_get_master_volume(device_, &result);

        return result;
    }

    bool AudioPlayer::SetVolume(float val) {
        if (!audioAsset_) {
            errorHandler_.SetError(ERROR_STR_AUDIO_NOT_OPENED);
            return false;
        }

        ma_device_set_master_volume(device_, val);

        return true;
    }
} // Uniasset