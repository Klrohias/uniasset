#pragma once
#ifndef UNIASSET_AUDIOUTILS_HPP
#define UNIASSET_AUDIOUTILS_HPP

#include <cstdint>
#include <miniaudio.h>
#include "uniasset/audio/SampleFormat.hpp"

namespace uniasset {
    inline uint64_t time2FrameOffset(float time, uint64_t sampleRate) {
        return static_cast<uint64_t>(time * sampleRate);
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
}

#endif // UNIASSET_AUDIOUTILS_HPP
