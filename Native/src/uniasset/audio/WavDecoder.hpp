//
// Created by Twiiz on 2024/1/21.
//

#pragma once
#ifndef UNIASSET_WAVDECODER_HPP
#define UNIASSET_WAVDECODER_HPP

#include <cstdlib>
#include <string_view>

#include "uniasset/audio/IAudioDecoder.hpp"

namespace uniasset {

    class WavDecoder : public IAudioDecoder {
        void* decoder_{nullptr};

    public:
        explicit WavDecoder(uint8_t* data, size_t len);

        explicit WavDecoder(const std::string_view& path);

        ~WavDecoder() override;

        WavDecoder(const WavDecoder&) = delete;

        WavDecoder& operator=(const WavDecoder&) = delete;

        uint32_t GetChannelCount() override;

        size_t GetSampleCount() override;

        SampleFormat GetSampleFormat() override;

        uint32_t GetSampleRate() override;

        bool Read(void* buffer, uint32_t count) override;
    };

} // Uniasset

#endif //UNIASSET_WAVDECODER_HPP
