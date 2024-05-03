//
// Created by NightOzZero on 2024/5/3.
//

#include "AudioDevice.hpp"

namespace uniasset {
    std::shared_ptr<AudioDevice> AudioDevice::instance_{};

    std::shared_ptr<AudioDevice> AudioDevice::getInstance() {
        if (instance_ == nullptr) {
            instance_ = std::make_shared<AudioDevice>();
        }

        return instance_;
    }

    static void audioDeviceDataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {

    }

    inline c_unique_ptr<ma_device, miniaudio_deleter>
    createDevice(ma_format preferFormat, AudioDevice *audioDeviceInstance) {
        auto config = ma_device_config_init(ma_device_type_playback);
        config.playback.format = preferFormat;
        config.playback.channels = 0;
        config.sampleRate = 0;
        config.pUserData = audioDeviceInstance;
        config.dataCallback = audioDeviceDataCallback;

        auto deviceMemory = std::make_unique<ma_device>();
        if (ma_device_init(nullptr, &config, deviceMemory.get()) != MA_SUCCESS) {
            return {nullptr, miniaudio_deleter};
        }

        return {deviceMemory.release(), miniaudio_deleter};
    }

    AudioDevice::AudioDevice() : device_{createDevice(ma_format_s16, this)} {

    }

} // uniasset