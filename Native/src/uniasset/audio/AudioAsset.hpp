//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_AUDIOASSET_HPP
#define UNIASSET_AUDIOASSET_HPP

#include <memory>
#include <vector>
#include <span>
#include <cmath>

#include "uniasset/Foundation.hpp"
#include "uniasset/common/Buffer.hpp"
#include "uniasset/common/Templates.hpp"
#include "uniasset/audio/SampleFormat.hpp"
#include "uniasset/common/Result.hpp"

namespace uniasset {

    class IAudioDecoder;

    enum LoadType : uint8_t {
        LoadType_None,
        LoadType_File,
        LoadType_Memory,
    };

    enum DataFormat : uint8_t {
        DataFormat_Pcm,
        DataFormat_Mp3,
        DataFormat_Ogg,
        DataFormat_Wav,
        DataFormat_Flac
    };

    class UNIASSET_API AudioAsset : public std::enable_shared_from_this<AudioAsset> {
    private:
        LoadType type_{LoadType_None};
        DataFormat format_{DataFormat_Pcm};
        size_t sampleCount_{0};
        uint32_t sampleRate_{0};
        uint32_t channelCount_{0};

        std::string path_{};
        Buffer data_{};
        size_t dataLength_{0};

        std::error_code loadMetadata();

    public:
        explicit AudioAsset();

        std::error_code load(const std::span<uint8_t>& data);

        std::error_code load(const std::string_view& path);

        void unload();

        Result<std::unique_ptr<IAudioDecoder>> getAudioDecoder(SampleFormat sampleFormat, int64_t frameBufferSize = -1);

        Result<size_t> getSampleCount();

        Result<uint32_t> getSampleRate();

        Result<uint32_t> getChannelCount();

        Result<float> getLength();

        LoadType getLoadType();

        Result<const std::string_view> getPath();

        const Buffer& getData();

        size_t getDataLength() const;
    };

} // Uniasset

#endif //UNIASSET_AUDIOASSET_HPP
