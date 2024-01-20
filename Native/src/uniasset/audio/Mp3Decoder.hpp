//
// Created by Twiiz on 2024/1/20.
//

#pragma once
#ifndef UNIASSET_MP3DECODER_HPP
#define UNIASSET_MP3DECODER_HPP

#include "uniasset/audio/IAudioDecoder.hpp"

namespace Uniasset {

    class Mp3Decoder : public IAudioDecoder {
    public:
        Mp3Decoder();

        ~Mp3Decoder();

        int32_t GetChannelCount() override;

        int32_t GetSampleCount() override;

        SampleFormat GetSampleFormat() override;

        int32_t GetSampleRate() override;
    };

} // Uniasset

#endif //UNIASSET_MP3DECODER_HPP
