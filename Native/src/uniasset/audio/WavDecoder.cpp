//
// Created by Twiiz on 2024/1/21.
//

#include "WavDecoder.hpp"

#include <dr_wav.h>

namespace uniasset {
    WavDecoder::WavDecoder(uint8_t* data, size_t len)
            : decoder_{new drwav} {
        if (!drwav_init_memory(reinterpret_cast<drwav*>(decoder_), data, len, nullptr)) {
            delete reinterpret_cast<drwav*>(decoder_);
            decoder_ = nullptr;
        }
    }

    WavDecoder::WavDecoder(const std::string_view& path)
            : decoder_{new drwav} {
        if (!drwav_init_file(reinterpret_cast<drwav*>(decoder_), path.data(), nullptr)) {
            delete reinterpret_cast<drwav*>(decoder_);
            decoder_ = nullptr;
        }
    }

    WavDecoder::~WavDecoder() {
        if (decoder_) {
            drwav_uninit(reinterpret_cast<drwav*>(decoder_));
            delete reinterpret_cast<drwav*>(decoder_);
        }
    }

    uint32_t WavDecoder::GetChannelCount() {
        if (!decoder_) return 0;
        return reinterpret_cast<drwav*>(decoder_)->channels;
    }

    size_t WavDecoder::GetSampleCount() {
        if (!decoder_) return 0;
        size_t result;
        drwav_get_length_in_pcm_frames(reinterpret_cast<drwav*>(decoder_), reinterpret_cast<drwav_uint64*>(&result));
        return result * GetChannelCount();
    }

    SampleFormat WavDecoder::GetSampleFormat() {
        return Int16;
    }

    uint32_t WavDecoder::GetSampleRate() {
        if (!decoder_) return 19200;
        return reinterpret_cast<drwav*>(decoder_)->sampleRate;
    }

    bool WavDecoder::Read(void* buffer, uint32_t count) {
        if (!decoder_) return false;
        drwav_read_pcm_frames_s16(reinterpret_cast<drwav*>(decoder_), count, reinterpret_cast<int16_t*>(buffer));
        return true;
    }

} // Uniasset