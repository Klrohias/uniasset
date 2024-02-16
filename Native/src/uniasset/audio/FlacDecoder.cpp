//
// Created by Twiiz on 2024/1/21.
//

#include "FlacDecoder.hpp"
#include "AudioAsset.hpp"

#include <dr_flac.h>
#include <utility>

namespace uniasset {
    FlacDecoder::FlacDecoder(std::shared_ptr<AudioAsset> asset)
            : asset_{std::move(asset)} {
        auto loadType = asset_->getLoadType();

        if (loadType == LoadType_Memory) {
            auto& data = asset_->getData();
            auto len = asset_->getDataLength();
            decoder_ = make_c_unique<drflac, drflac_deleter>(drflac_open_memory(data.get(), len, nullptr));
        } else if (loadType == LoadType_File) {
            auto& path = asset_->getPath();

            decoder_ = make_c_unique<drflac, drflac_deleter>(drflac_open_file(path.data(), nullptr));
        }
    }

    uint32_t FlacDecoder::getChannelCount() {
        if (!decoder_) return 1;
        return decoder_->channels;
    }

    size_t FlacDecoder::getSampleCount() {
        if (!decoder_) return 0;
        return decoder_->totalPCMFrameCount * getChannelCount();
    }

    SampleFormat FlacDecoder::getSampleFormat() {
        return Int16;
    }

    uint32_t FlacDecoder::getSampleRate() {
        if (!decoder_) return 19200;
        return decoder_->sampleRate;
    }

    bool FlacDecoder::read(void* buffer, uint32_t count) {
        if (!decoder_) return false;
        drflac_read_pcm_frames_s16(decoder_.get(), count, reinterpret_cast<int16_t*>(buffer));
        return true;
    }
} // Uniasset