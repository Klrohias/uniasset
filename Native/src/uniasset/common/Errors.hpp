//
// Created by qingy on 2024/2/16.
//

#pragma once
#ifndef UNIASSET_ERRORS_HPP
#define UNIASSET_ERRORS_HPP

#include <system_error>

namespace uniasset {
    enum Error : int {
        kSuccess = 0,
        kWebPInfoReadFail = 1,
        kWebPDecoderInitFail = 2,
        kTurboJpegInitFail = 3,
        kImageNotLoadFail = 4,
        kRectOverflow = 5,
        kAudioNotLoadFail = 6,
        kNotSupportedFail = 7,
        kAudioDeviceCreateFail = 8,
        kAudioDeviceClosed = 9,
    };

    const std::error_category& uniasset_category();
    const std::error_category& vp8_category();
    const std::error_category& stbi_category();
    const std::error_category& turbojpeg_category();

    inline std::error_code err_ok() {
        return std::error_code{0, std::generic_category()};
    }

    inline std::error_code err_errno() {
        return std::error_code{errno, std::generic_category()};
    }
}

#endif //UNIASSET_ERRORS_HPP
