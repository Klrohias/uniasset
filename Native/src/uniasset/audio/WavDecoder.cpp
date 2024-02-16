//
// Created by Twiiz on 2024/1/21.
//

#include "WavDecoder.hpp"
#include "AudioAsset.hpp"

#include <span>
#include <utility>

namespace uniasset {

    inline c_unique_ptr<drwav, drwav_deleter> createDrwavDecoder(const std::span<uint8_t>& data) {
        auto result{make_c_unique<drwav, drwav_deleter>(new drwav{})};

        if (!drwav_init_memory(result.get(), data.data(), data.size(), nullptr)) {
            result.reset();
        }

        return result;
    }

    inline c_unique_ptr<drwav, drwav_deleter> createDrwavDecoder(const std::string_view& path) {
        auto result{make_c_unique<drwav, drwav_deleter>(new drwav{})};

        if (!drwav_init_file(result.get(), path.data(), nullptr)) {
            result.reset();
        }

        return result;
    }

    WavDecoder::WavDecoder(std::shared_ptr<AudioAsset> asset)
            : asset_{std::move(asset)} {
        auto loadType = asset_->getLoadType();

        if (loadType == LoadType_Memory) {
            auto& data = asset_->getData();
            auto len = asset_->getDataLength();
            decoder_ = createDrwavDecoder(std::span<uint8_t>(data.get(), len));
        } else if (loadType == LoadType_File) {
            auto& path = asset_->getPath();
            decoder_ = createDrwavDecoder(path);
        }
    }

    uint32_t WavDecoder::getChannelCount() {
        if (!decoder_) return 0;
        return decoder_->channels;
    }

    size_t WavDecoder::getSampleCount() {
        if (!decoder_) return 0;
        size_t result;
        drwav_get_length_in_pcm_frames(decoder_.get(), reinterpret_cast<drwav_uint64*>(&result));
        return result * getChannelCount();
    }

    SampleFormat WavDecoder::getSampleFormat() {
        return Int16;
    }

    uint32_t WavDecoder::getSampleRate() {
        if (!decoder_) return 19200;
        return decoder_->sampleRate;
    }

    bool WavDecoder::read(void* buffer, uint32_t count) {
        if (!decoder_) return false;
        drwav_read_pcm_frames_s16(decoder_.get(), count, reinterpret_cast<int16_t*>(buffer));
        return true;
    }

} // Uniasset