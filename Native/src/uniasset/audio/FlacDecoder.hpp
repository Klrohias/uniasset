//
// Created by Twiiz on 2024/1/21.
//

#pragma once
#ifndef UNIASSET_FLACDECODER_HPP
#define UNIASSET_FLACDECODER_HPP

#include <cstdlib>
#include <string_view>

#include "uniasset/audio/IAudioDecoder.hpp"

namespace uniasset {

    class FlacDecoder : public IAudioDecoder{
        void* decoder_{nullptr};

    public:
        explicit FlacDecoder(uint8_t* data, size_t len);

        explicit FlacDecoder(const std::string_view& path);

        ~FlacDecoder() override;

        FlacDecoder(const FlacDecoder&) = delete;

        FlacDecoder& operator=(const FlacDecoder&) = delete;

        uint32_t GetChannelCount() override;

        size_t GetSampleCount() override;

        SampleFormat GetSampleFormat() override;

        uint32_t GetSampleRate() override;

        bool Read(void* buffer, uint32_t count) override;
    };

} // Uniasset

#endif //UNIASSET_FLACDECODER_HPP
