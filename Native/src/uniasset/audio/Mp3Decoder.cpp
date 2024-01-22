//
// Created by Twiiz on 2024/1/20.
//

#include "Mp3Decoder.hpp"

#include <dr_mp3.h>

namespace Uniasset {
    Mp3Decoder::Mp3Decoder(uint8_t* data, size_t len)
            : decoder_{new drmp3} {
        if (!drmp3_init_memory(reinterpret_cast<drmp3*>(decoder_), data, len, nullptr)) {
            delete reinterpret_cast<drmp3*>(decoder_);
            decoder_ = nullptr;
        }
    }

    Mp3Decoder::Mp3Decoder(const std::string_view& path)
            : decoder_{new drmp3} {
        if (!drmp3_init_file(reinterpret_cast<drmp3*>(decoder_), path.data(), nullptr)) {
            delete reinterpret_cast<drmp3*>(decoder_);
            decoder_ = nullptr;
        }
    }

    Mp3Decoder::~Mp3Decoder() {
        if (decoder_) {
            drmp3_uninit((drmp3*) decoder_);
            delete reinterpret_cast<drmp3*>(decoder_);
        }
    }

    uint32_t Mp3Decoder::GetChannelCount() {
        if (!decoder_) return 1;
        return reinterpret_cast<drmp3*>(decoder_)->channels;
    }

    size_t Mp3Decoder::GetSampleCount() {
        if (!decoder_) return 0;
        drmp3_uint64 mp3FrameCount;
        drmp3_uint64 mp3PcmCount;
        drmp3_get_mp3_and_pcm_frame_count(reinterpret_cast<drmp3*>(decoder_), &mp3FrameCount, &mp3PcmCount);
        return mp3PcmCount * GetChannelCount();
    }

    SampleFormat Mp3Decoder::GetSampleFormat() {
        return Int16;
    }

    uint32_t Mp3Decoder::GetSampleRate() {
        if (!decoder_) return 19200;
        return reinterpret_cast<drmp3*>(decoder_)->sampleRate;
    }

    bool Mp3Decoder::Read(void* buffer, uint32_t count) {
        if (!decoder_) return false;
        drmp3_read_pcm_frames_s16(reinterpret_cast<drmp3*>(decoder_), count, static_cast<int16_t*>(buffer));
        return true;
    }
} // Uniasset