//
// Created by NightOzZero on 2024/5/3.
//

#pragma once

#ifndef UNIASSET_AUDIODEVICE_HPP
#define UNIASSET_AUDIODEVICE_HPP

#include <memory>
#include "uniasset/Foundation.hpp"
#include "uniasset/thirdparty/miniaudio.hpp"

namespace uniasset {

    class UNIASSET_API AudioDevice {
    private:
        static std::shared_ptr<AudioDevice> instance_;
        c_unique_ptr<ma_device, miniaudio_deleter> device_;

    public:
        static std::shared_ptr<AudioDevice> getInstance();

        explicit AudioDevice();

    };

} // uniasset

#endif //UNIASSET_AUDIODEVICE_HPP
