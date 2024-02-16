//
// Created by Twiiz on 2024/1/21.
//

#include "OggDecoder.hpp"
#include "AudioAsset.hpp"

#include <utility>

namespace uniasset {
    OggDecoder::OggDecoder(std::shared_ptr<AudioAsset> asset, SampleFormat sampleFormat)
            : asset_{std::move(asset)}, sampleFormat_{sampleFormat} {
        auto loadType = asset_->getLoadType();

        if (loadType == LoadType_Memory) {
            auto& data = asset_->getData();
            auto len = asset_->getDataLength();
            decoder_ = make_c_unique<stb_vorbis, stb_vorbis_deleter>(
                    stb_vorbis_open_memory(data.get(), len, &loadError_, nullptr));
        } else if (loadType == LoadType_File) {
            auto& path = asset_->getPath();

            decoder_ = make_c_unique<stb_vorbis, stb_vorbis_deleter>(
                    stb_vorbis_open_filename(path.data(), &loadError_, nullptr));
        }

        if (decoder_) {
            info_ = stb_vorbis_get_info(decoder_.get());
        }
    }

    SampleFormat OggDecoder::getSampleFormat() {
        return SampleFormat_Int16;
    }

    uint32_t OggDecoder::getChannelCount() {
        if (!decoder_) return 1;
        return info_.channels;
    }

    uint32_t OggDecoder::getSampleRate() {
        if (!decoder_) return 19200;
        return info_.sample_rate;
    }

    size_t OggDecoder::getSampleCount() {
        if (!decoder_) return 0;
        return stb_vorbis_stream_length_in_samples(decoder_.get()) * getChannelCount();
    }

    bool OggDecoder::read(void* buffer, uint32_t count) {
        if (!decoder_) return false;
        auto channels = info_.channels;

        if (sampleFormat_ == SampleFormat_Int16) {
            stb_vorbis_get_samples_short_interleaved(decoder_.get(), channels,
                                                     reinterpret_cast<int16_t*>(buffer), count * channels);
        } else {
            stb_vorbis_get_samples_float_interleaved(decoder_.get(), channels,
                                                     reinterpret_cast<float_t*>(buffer), count * channels);
        }

        return true;
    }

    int OggDecoder::getLoadError() const {
        return loadError_;
    }

    bool OggDecoder::seek(int64_t position) {
        if (!decoder_) return false;
        return stb_vorbis_seek(decoder_.get(), position);
    }

    int64_t OggDecoder::tell() {
        if (!decoder_) return 0;
        return stb_vorbis_get_sample_offset(decoder_.get());
    }
} // Uniasset