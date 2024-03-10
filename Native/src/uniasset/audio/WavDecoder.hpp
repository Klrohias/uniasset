//
// Created by Twiiz on 2024/1/21.
//

#pragma once
#ifndef UNIASSET_WAVDECODER_HPP
#define UNIASSET_WAVDECODER_HPP

#include <cstdlib>
#include <string_view>
#include <memory>
#include <dr_wav.h>

#include "uniasset/audio/IAudioDecoder.hpp"
#include "uniasset/thirdparty/unique.hpp"

namespace uniasset {
    class AudioAsset;

    inline void drwav_deleter(drwav* decoder) {
        drwav_uninit(decoder);
    }

    class WavDecoder : public IAudioDecoder {
        std::shared_ptr<AudioAsset> asset_{};
        c_unique_ptr<drwav, drwav_deleter> decoder_{nullptr, drwav_deleter};
        SampleFormat sampleFormat_{SampleFormat_Int16};

    public:
        explicit WavDecoder(std::shared_ptr<AudioAsset> asset, SampleFormat sampleFormat);

        WavDecoder(WavDecoder&&) = default;

        ~WavDecoder() override = default;

        uint32_t getChannelCount() override;

        size_t getSampleCount() override;

        SampleFormat getSampleFormat() override;

        uint32_t getSampleRate() override;

        uint32_t read(void* buffer, uint32_t count) override;

        bool seek(int64_t position) override;

        int64_t tell() override;
    };

} // Uniasset

#endif //UNIASSET_WAVDECODER_HPP
