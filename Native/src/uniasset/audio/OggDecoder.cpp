//
// Created by Twiiz on 2024/1/21.
//

#include "OggDecoder.hpp"

#include <iostream>
#include <stb_vorbis.c>

namespace uniasset {
    OggDecoder::OggDecoder(uint8_t* data, size_t len) {
        decoder_ = stb_vorbis_open_memory(data, len, &loadError_, nullptr);
        if (loadError_ != 0) {
            std::cerr << "Failed to open ogg file: " << loadError_ << "\n";
        }
    }

    OggDecoder::OggDecoder(const std::string_view& path) {
        decoder_ = stb_vorbis_open_filename(path.data(), &loadError_, nullptr);
        if (loadError_ != 0) {
            std::cerr << "Failed to open ogg file: " << loadError_ << "\n";
        }
    }

    OggDecoder::~OggDecoder() {
        if (decoder_) {
            stb_vorbis_close(reinterpret_cast<stb_vorbis*>(decoder_));
        }
    }

    SampleFormat OggDecoder::GetSampleFormat() {
        return Int16;
    }

    uint32_t OggDecoder::GetChannelCount() {
        if (!decoder_) return 1;
        return reinterpret_cast<stb_vorbis*>(decoder_)->channels;
    }

    uint32_t OggDecoder::GetSampleRate() {
        if (!decoder_) return 19200;
        return reinterpret_cast<stb_vorbis*>(decoder_)->sample_rate;
    }

    size_t OggDecoder::GetSampleCount() {
        if (!decoder_) return 0;
        return stb_vorbis_stream_length_in_samples(reinterpret_cast<stb_vorbis*>(decoder_)) * GetChannelCount();
    }

    bool OggDecoder::Read(void* buffer, uint32_t count) {
        if (!decoder_) return false;
        auto channels = reinterpret_cast<stb_vorbis*>(decoder_)->channels;
        stb_vorbis_get_samples_short_interleaved(reinterpret_cast<stb_vorbis*>(decoder_), channels,
                                                 reinterpret_cast<int16*>(buffer), count * channels);
        return true;
    }
} // Uniasset