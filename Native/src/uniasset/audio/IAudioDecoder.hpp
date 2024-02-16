//
// Created by Twiiz on 2024/1/20.
//

#pragma once
#ifndef UNIASSET_IAUDIODECODER_HPP
#define UNIASSET_IAUDIODECODER_HPP

#include <cstdint>
#include <cstdio>
#include <memory>

#include "uniasset/Foundation.hpp"
#include "uniasset/audio/SampleFormat.hpp"

namespace uniasset {
    class IAudioDecoder : public std::enable_shared_from_this<IAudioDecoder> {
    public:
        virtual ~IAudioDecoder() = default;

        virtual SampleFormat getSampleFormat() = 0;

        virtual uint32_t getChannelCount() = 0;

        virtual size_t getSampleCount() = 0;

        virtual uint32_t getSampleRate() = 0;

        virtual bool seek(int64_t position) = 0;

        virtual int64_t tell() = 0;

        virtual bool read(void* buffer, uint32_t count) = 0;
    };

} // Uniasset

#endif //UNIASSET_IAUDIODECODER_HPP
