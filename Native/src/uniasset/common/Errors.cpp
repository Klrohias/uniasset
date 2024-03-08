//
// Created by qingy on 2024/2/16.
//

#include "Errors.hpp"

namespace uniasset {
    const char* ERROR_STR_AUDIO_NOT_SUPPORTED = "audio format is not supported";
    const char* ERROR_STR_AUDIO_METADATA = "failed to read audio metadata";
    const char* ERROR_STR_AUDIO_NOT_LOADED = "audio asset is not loaded";
    const char* ERROR_STR_AUDIO_NOT_OPENED = "audio player has not opened any audio asset";
    const char* ERROR_STR_IMAGE_NOT_LOADED = "image asset is not loaded";
    const char* ERROR_STR_IMAGE_SIZE_OVERFLOW = "range exceeds image size";
    const char* ERROR_STR_IMAGE_SIZE_OVERFLOW_AT = "range exceeds image size at item {}";
}