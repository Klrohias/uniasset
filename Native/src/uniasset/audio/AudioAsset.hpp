//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_AUDIOASSET_HPP
#define UNIASSET_AUDIOASSET_HPP

#include <memory>
#include <vector>
#include <span>

#include "uniasset/Foundation.hpp"
#include "uniasset/common/Buffer.hpp"
#include "uniasset/common/Templates.hpp"
#include "uniasset/utils/ErrorHandler.hpp"
#include "uniasset/audio/SampleFormat.hpp"

namespace uniasset {
    using namespace uniasset::utils;

    class IAudioDecoder;

    class AudioPlayer;

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

        ErrorHandler errorHandler_{};

        bool loadMetadata();

    public:
        explicit AudioAsset();

        void load(const std::span<uint8_t>& data);

        void load(const std::string_view& path);

        void unload();

        std::unique_ptr<IAudioDecoder> getAudioDecoder();

        const std::string& getError();

        size_t getSampleCount();

        uint32_t getSampleRate();

        uint32_t getChannelCount();

        float getLength();

        LoadType getLoadType();

        const std::string& getPath();

        const Buffer& getData();

        size_t getDataLength();
    };

} // Uniasset

#endif //UNIASSET_AUDIOASSET_HPP
