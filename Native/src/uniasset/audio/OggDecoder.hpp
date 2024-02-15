//
// Created by Twiiz on 2024/1/21.
//

#pragma once
#ifndef UNIASSET_OGGDECODER_HPP
#define UNIASSET_OGGDECODER_HPP

#include <cstdlib>
#include <string_view>

#include "uniasset/audio/IAudioDecoder.hpp"

namespace uniasset {

    class OggDecoder : public IAudioDecoder {
        void* decoder_{nullptr};
        int loadError_{};

    public:
        explicit OggDecoder(uint8_t* data, size_t len);

        explicit OggDecoder(const std::string_view& path);

        ~OggDecoder() override;

        OggDecoder(const OggDecoder&) = delete;

        OggDecoder& operator=(const OggDecoder&) = delete;

        uint32_t GetChannelCount() override;

        size_t GetSampleCount() override;

        SampleFormat GetSampleFormat() override;

        uint32_t GetSampleRate() override;

        bool Read(void* buffer, uint32_t count) override;
    };

} // Uniasset

#endif //UNIASSET_OGGDECODER_HPP
