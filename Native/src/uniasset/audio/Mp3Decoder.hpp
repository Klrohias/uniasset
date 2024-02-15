//
// Created by Twiiz on 2024/1/20.
//

#pragma once
#ifndef UNIASSET_MP3DECODER_HPP
#define UNIASSET_MP3DECODER_HPP

#include <cstdlib>
#include <string_view>

#include "uniasset/audio/IAudioDecoder.hpp"

namespace uniasset {

    class Mp3Decoder : public IAudioDecoder {
        void* decoder_{nullptr};

    public:
        explicit Mp3Decoder(uint8_t* data, size_t len);

        explicit Mp3Decoder(const std::string_view& path);

        ~Mp3Decoder() override;

        Mp3Decoder(const Mp3Decoder&) = delete;

        Mp3Decoder& operator=(const Mp3Decoder&) = delete;

        uint32_t GetChannelCount() override;

        size_t GetSampleCount() override;

        SampleFormat GetSampleFormat() override;

        uint32_t GetSampleRate() override;

        bool Read(void* buffer, uint32_t count) override;
    };

} // Uniasset

#endif //UNIASSET_MP3DECODER_HPP
