//
// Created by dogdie233 on 2024/8/28.
//

#include "AudioPlayer.hpp"

#include <cstring>
#include <algorithm>
#include <limits>

#include "uniasset/audio/AudioAsset.hpp"
#include "uniasset/audio/player/IWaveProvider.hpp"
#include <uniasset/common/Errors.hpp>
#include <uniasset/common/AudioUtils.hpp>

namespace uniasset {
    void AudioPlayer::maDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, uint32_t frameCount) {
        (void)pInput;

        auto player = reinterpret_cast<AudioPlayer*>(pDevice->pUserData);
        auto format = pDevice->playback.format;
        auto channel = pDevice->playback.channels;
        auto sampleRate = pDevice->sampleRate;
        auto frameSize = ma_get_bytes_per_frame(format, channel);

        if (player->state_ == Playing) {
            // passedSampleCount is used for time calculation
            // only increase when playing
            player->passedFrameCount_ += frameCount;
        }
        if (player->state_ != Playing || player->playingAudios_.empty()) {
            // not playing or no any playing audio, fill zero
            memset(pOutput, 0, frameSize * frameCount);
            return;
        }

        // read frame
        player->mixer_.begin(pOutput, frameCount);
        for (auto &audio : player->playingAudios_) {
            if (audio.handler->getState() != PlayingHandler::State::Playing) {
				continue;
			}
            IWaveProvider::ReadResult readResult;
            audio.waveProvider->read(readResult, time2FrameOffset(audio.handler->getTime(), sampleRate), frameCount);
            player->mixer_.mix(readResult);
        }
        player->mixer_.end();
    }

    AudioPlayer::AudioPlayer() = default;

    AudioPlayer::~AudioPlayer() = default;

    std::error_code AudioPlayer::reset() {
        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.aaudio.usage = ma_aaudio_usage_game;
        deviceConfig.aaudio.contentType = ma_aaudio_content_type_music;
        deviceConfig.pUserData = this;
        deviceConfig.dataCallback = &maDataCallback;

        device_ = miniaudio_create_device(deviceConfig);
        if (!device_) {
            state_ = Closed;
            return std::error_code{ kAudioDeviceCreateFail, uniasset_category() };
        }

        ma_device_start(device_.get());
        ma_device_set_master_volume(device_.get(), volume_);
        printf("Reset audio playback device, sampleRate: %u, channel: %u", device_->playback.channels, device_->sampleRate);
        state_ = Paused;
        return err_ok();
    }

    /*
    void AudioPlayer::open(const std::shared_ptr<AudioAsset>& audioAsset) {
        errorHandler_.clear();

        // get decoder
        auto audioDecoder = audioAsset->getAudioDecoder(SampleFormat_Int16);
        auto sharedAudioDecoder = std::shared_ptr<IAudioDecoder>(audioDecoder.release());
        open(sharedAudioDecoder);
    }

    void AudioPlayer::open(const std::shared_ptr<IAudioDecoder>& audioDecoder) {
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

        audioDecoder_ = audioDecoder;

        // set state
        state_ = Opened;
        decodedSampleCount_ = 0;
        channelCount_ = audioDecoder_->getChannelCount();
        sampleRate_ = audioDecoder_->getSampleRate();
    }
    */

    void AudioPlayer::close() {
        if (state_ == Closed) {
            return;
        }

        // reset states
        state_ = Closed;
        passedFrameCount_ = 0;
        playingAudios_.clear();

        device_.reset();
    }

    Result<std::shared_ptr<PlayingHandler>> AudioPlayer::play(const std::shared_ptr<IWaveProvider>& waveProvider) {
        if (state_ == Closed) {
			return std::error_code{ kAudioDeviceClosed , uniasset_category() };
		}
        
        auto handler = std::make_shared<PlayingHandler>();
        waveProvider->init(AudioConfig(this->device_->playback.channels, toSampleFormat(this->device_->playback.format), this->device_->sampleRate));

        this->playingAudios_.push_back({ handler, waveProvider });

        return handler;
    }

    std::error_code AudioPlayer::pause() {
        if (state_ == Closed) {
            return std::error_code{ kAudioDeviceClosed , uniasset_category() };
		}
        state_ = Paused;
        return err_ok();
    }

    std::error_code AudioPlayer::resume() {
        if (state_ == Closed) {
            return std::error_code{ kAudioDeviceClosed , uniasset_category() };
        }
        state_ = Playing;
        return err_ok();
    }

    Result<AudioPlayer::State> AudioPlayer::getState() {
		return state_;
	}

    Result<float> AudioPlayer::getVolume() {
        return volume_;
    }

    std::error_code AudioPlayer::setVolume(float val) {
        volume_ = val;

        if (state_ == Closed) {
            return err_ok();
        }

        ma_device_set_master_volume(device_.get(), val);
        return err_ok();
    }

    Result<double> AudioPlayer::getPassedTime() {
        if (state_ == Closed) {
            return 0.0;
        }
        auto sampleRate = device_->playback.internalSampleRate;
        return static_cast<double>(passedFrameCount_ / sampleRate)
            + (static_cast<double>(passedFrameCount_ % sampleRate) / sampleRate);
    }
} // Uniasset