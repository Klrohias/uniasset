//
// Created by Twiiz on 2024/1/21.
//

#pragma once
#ifndef UNIASSET_OGGDECODER_HPP
#define UNIASSET_OGGDECODER_HPP

#include <cstdlib>
#include <string_view>
#include <memory>

#include "uniasset/audio/IAudioDecoder.hpp"
#include "uniasset/thirdparty/stb_vorbis.hpp"
#include "uniasset/thirdparty/unique.hpp"

namespace uniasset {
    class AudioAsset;

    inline void stb_vorbis_deleter(stb_vorbis* decoder) {
        stb_vorbis_close(decoder);
    }

    class OggDecoder : public IAudioDecoder {
        std::shared_ptr<AudioAsset> asset_{};
        c_unique_ptr<stb_vorbis, stb_vorbis_deleter> decoder_{nullptr, stb_vorbis_deleter};
        SampleFormat sampleFormat_{SampleFormat_Int16};

        stb_vorbis_info info_{};
        int loadError_{};

    public:
        explicit OggDecoder(std::shared_ptr<AudioAsset> asset, SampleFormat sampleFormat);

        OggDecoder(OggDecoder&&) = default;

        ~OggDecoder() override = default;

        uint32_t getChannelCount() override;

        size_t getSampleCount() override;

        SampleFormat getSampleFormat() override;

        uint32_t getSampleRate() override;

        uint32_t read(void* buffer, uint32_t count) override;

        int getLoadError() const;

        bool seek(int64_t position) override;

        int64_t tell() override;
    };

} // Uniasset

#endif //UNIASSET_OGGDECODER_HPP
