#pragma once
#ifndef UNIASSET_AUDIOUTILS_H
#define UNIASSET_AUDIOUTILS_H

#include <cstdint>
#include <miniaudio.h>
#include "uniasset/audio/SampleFormat.hpp"

namespace uniasset {
    inline uint32_t time2FrameOffset(float time, uint32_t sampleRate) {
        return static_cast<uint32_t>(time * sampleRate);
    }

    inline size_t calcSampleSize(SampleFormat format) {
		switch (format) {
		case SampleFormat_Float:
			return sizeof(float);
		case SampleFormat_Int16:
			return sizeof(int16_t);
		}
		return 0;
	}

    inline size_t calcFrameSize(SampleFormat format, uint32_t channelCount) {
		return calcSampleSize(format) * channelCount;
	}

    inline ma_format toMaFormat(SampleFormat format) {
        switch (format) {
        case SampleFormat_Float:
            return ma_format_f32;
        case SampleFormat_Int16:
            return ma_format_s16;
        }
        return ma_format_u8;
    }

    inline SampleFormat toSampleFormat(ma_format format) {
        switch (format) {
        case ma_format_f32:
            return SampleFormat_Float;
        case ma_format_s16:
            return SampleFormat_Int16;
        }
        // IDK how to handle other case
    }
}

#endif // UNIASSET_AUDIOUTILS_H
