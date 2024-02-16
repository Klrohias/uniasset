//
// Created by qingy on 2024/2/16.
//

#pragma once
#ifndef UNIASSET_MINIAUDIO_HPP
#define UNIASSET_MINIAUDIO_HPP

#include "uniasset/thirdparty/unique.hpp"
#include <memory>
#include <miniaudio.h>

namespace uniasset {
    inline void miniaudio_deleter(ma_device* device) {
        ma_device_uninit(device);
    }

    inline c_unique_ptr<ma_device, miniaudio_deleter> miniaudio_create_device(ma_device_config& config) {
        auto result{make_c_unique<ma_device, miniaudio_deleter>(new ma_device{})};
        if (ma_device_init(nullptr, &config, result.get()) != MA_SUCCESS) {
            result.reset();
        }

        return result;
    }
}

#endif //UNIASSET_MINIAUDIO_HPP
