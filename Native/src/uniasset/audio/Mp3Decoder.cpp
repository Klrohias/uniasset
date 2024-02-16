//
// Created by Twiiz on 2024/1/20.
//

#include "Mp3Decoder.hpp"
#include "AudioAsset.hpp"

#include <dr_mp3.h>
#include <utility>
#include <span>

namespace uniasset {

    inline c_unique_ptr<drmp3, drmp3_deleter> createDrmp3Decoder(const std::span<uint8_t>& data) {
        auto result{make_c_unique<drmp3, drmp3_deleter>(new drmp3{})};

        if (!drmp3_init_memory(result.get(), data.data(), data.size(), nullptr)) {
            result.reset();
        }

        return result;
    }

    inline c_unique_ptr<drmp3, drmp3_deleter> createDrmp3Decoder(const std::string_view& path) {
        auto result{make_c_unique<drmp3, drmp3_deleter>(new drmp3{})};

        if (!drmp3_init_file(result.get(), path.data(), nullptr)) {
            result.reset();
        }

        return result;
    }

    Mp3Decoder::Mp3Decoder(std::shared_ptr<AudioAsset> asset)
            : asset_{std::move(asset)} {
        auto loadType = asset_->getLoadType();

        if (loadType == LoadType_Memory) {
            auto& data = asset_->getData();
            auto len = asset_->getDataLength();
            decoder_ = createDrmp3Decoder(std::span<uint8_t>(data.get(), len));
        } else if (loadType == LoadType_File) {
            auto& path = asset_->getPath();
            decoder_ = createDrmp3Decoder(path);
        }
    }

    uint32_t Mp3Decoder::getChannelCount() {
        if (!decoder_) return 1;
        return decoder_->channels;
    }

    size_t Mp3Decoder::getSampleCount() {
        if (!decoder_) return 0;
        drmp3_uint64 mp3FrameCount;
        drmp3_uint64 mp3PcmCount;
        drmp3_get_mp3_and_pcm_frame_count(decoder_.get(), &mp3FrameCount, &mp3PcmCount);
        return mp3PcmCount * getChannelCount();
    }

    SampleFormat Mp3Decoder::getSampleFormat() {
        return Int16;
    }

    uint32_t Mp3Decoder::getSampleRate() {
        if (!decoder_) return 19200;
        return decoder_->sampleRate;
    }

    bool Mp3Decoder::read(void* buffer, uint32_t count) {
        if (!decoder_) return false;
        drmp3_read_pcm_frames_s16(decoder_.get(), count, static_cast<int16_t*>(buffer));
        return true;
    }
} // Uniasset
