//
// Created by Twiiz on 2024/1/20.
//

#pragma once
#ifndef UNIASSET_MP3DECODER_HPP
#define UNIASSET_MP3DECODER_HPP

#include <cstdlib>
#include <string_view>
#include <memory>
#include <dr_mp3.h>

#include "uniasset/audio/IAudioDecoder.hpp"
#include "uniasset/thirdparty/unique.hpp"

namespace uniasset {
    class AudioAsset;

    inline void drmp3_deleter(drmp3* decoder) {
        drmp3_uninit(decoder);
        delete decoder;
    }

    class Mp3Decoder : public IAudioDecoder {
        std::shared_ptr<AudioAsset> asset_{};
        c_unique_ptr<drmp3, drmp3_deleter> decoder_{nullptr, drmp3_deleter};
        SampleFormat sampleFormat_{SampleFormat_Int16};

    public:
        explicit Mp3Decoder(std::shared_ptr<AudioAsset> asset, SampleFormat sampleFormat);

        Mp3Decoder(Mp3Decoder&&) = default;

        ~Mp3Decoder() override = default;

        uint32_t getChannelCount() override;

        size_t getSampleCount() override;

        SampleFormat getSampleFormat() override;

        uint32_t getSampleRate() override;

        bool read(void* buffer, uint32_t count) override;

        bool seek(int64_t position) override;

        int64_t tell() override;
    };

} // Uniasset

#endif //UNIASSET_MP3DECODER_HPP
