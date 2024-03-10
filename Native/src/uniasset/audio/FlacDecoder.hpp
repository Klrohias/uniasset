//
// Created by Twiiz on 2024/1/21.
//

#pragma once
#ifndef UNIASSET_FLACDECODER_HPP
#define UNIASSET_FLACDECODER_HPP

#include <cstdlib>
#include <string_view>
#include <dr_flac.h>

#include "uniasset/Foundation.hpp"
#include "uniasset/thirdparty/unique.hpp"
#include "uniasset/audio/IAudioDecoder.hpp"

namespace uniasset {
    class AudioAsset;

    inline void drflac_deleter(drflac* decoder) {
        drflac_close(decoder);
    }

    class UNIASSET_API FlacDecoder : public IAudioDecoder {
        std::shared_ptr<AudioAsset> asset_{nullptr};
        c_unique_ptr<drflac, drflac_deleter> decoder_{nullptr, drflac_deleter};
        SampleFormat sampleFormat_{SampleFormat_Int16};

    public:
        explicit FlacDecoder(std::shared_ptr<AudioAsset> asset, SampleFormat sampleFormat);

        FlacDecoder(FlacDecoder&&) = default;

        ~FlacDecoder() override = default;

        uint32_t getChannelCount() override;

        size_t getSampleCount() override;

        SampleFormat getSampleFormat() override;

        uint32_t getSampleRate() override;

        uint32_t read(void* buffer, uint32_t count) override;

        bool seek(int64_t position) override;

        int64_t tell() override;
    };

} // Uniasset

#endif //UNIASSET_FLACDECODER_HPP
