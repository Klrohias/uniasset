//
// Created by Twiiz on 2024/1/20.
//

#pragma once
#ifndef UNIASSET_IAUDIODECODER_HPP
#define UNIASSET_IAUDIODECODER_HPP

#include <cstdint>
#include <cstdio>

#include "uniasset/Foundation.hpp"
#include "uniasset/audio/SampleFormat.hpp"

namespace uniasset {
    class IAudioDecoder {
    public:
        virtual ~IAudioDecoder() = default;

        virtual SampleFormat GetSampleFormat() = 0;

        virtual uint32_t GetChannelCount() = 0;

        virtual size_t GetSampleCount() = 0;

        virtual uint32_t GetSampleRate() = 0;

        virtual bool Read(void* buffer, uint32_t count) = 0;
    };

} // Uniasset

#endif //UNIASSET_IAUDIODECODER_HPP
