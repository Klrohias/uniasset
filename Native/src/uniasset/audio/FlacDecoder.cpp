//
// Created by Twiiz on 2024/1/21.
//

#include "FlacDecoder.hpp"

#include <dr_flac.h>

namespace uniasset {
    FlacDecoder::FlacDecoder(uint8_t* data, size_t len) {
        decoder_ = drflac_open_memory(data, len, nullptr);
    }

    FlacDecoder::FlacDecoder(const std::string_view& path) {
        decoder_ = drflac_open_file(path.data(), nullptr);
    }

    FlacDecoder::~FlacDecoder() {
        if (decoder_) {
            drflac_close(reinterpret_cast<drflac*>(decoder_));
        }
    }

    uint32_t FlacDecoder::GetChannelCount() {
        if (!decoder_) return 1;
        return reinterpret_cast<drflac*>(decoder_)->channels;
    }

    size_t FlacDecoder::GetSampleCount() {
        if (!decoder_) return 0;
        return reinterpret_cast<drflac*>(decoder_)->totalPCMFrameCount * GetChannelCount();
    }

    SampleFormat FlacDecoder::GetSampleFormat() {
        return Int16;
    }

    uint32_t FlacDecoder::GetSampleRate() {
        if (!decoder_) return 19200;
        return reinterpret_cast<drflac*>(decoder_)->sampleRate;
    }

    bool FlacDecoder::Read(void* buffer, uint32_t count) {
        if (!decoder_) return false;
        drflac_read_pcm_frames_s16(reinterpret_cast<drflac*>(decoder_), count, reinterpret_cast<int16_t*>(buffer));
        return true;
    }
} // Uniasset