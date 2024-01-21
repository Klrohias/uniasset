//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_AUDIOASSET_HPP
#define UNIASSET_AUDIOASSET_HPP

#include <memory>
#include <vector>

#include "uniasset/Foundation.hpp"
#include "uniasset/common/ErrorHandler.hpp"
#include "uniasset/audio/SampleFormat.hpp"

namespace Uniasset {
    class IAudioDecoder;

    class AudioPlayer;

    class UNIASSET_API AudioAsset {
    private:
        enum LoadType {
            None,
            File,
            Memory,
        };

        enum DataFormat {
            Pcm,
            Mp3,
            Ogg,
            Wav,
            Flac
        };

        struct AudioAssetLoadInfo {
            LoadType type{None};
            std::string path{};
            uint8_t* data{nullptr};
            size_t dataLength{0};
            DataFormat format{Pcm};
        };

        std::vector<AudioPlayer*> playingAudioPlayer_{};
        ErrorHandler errorHandler_{};
        AudioAssetLoadInfo loadInfo_{};

        void Cleanup();

    public:

        explicit AudioAsset();

        ~AudioAsset();

        AudioAsset(const AudioAsset&) = delete;

        AudioAsset& operator=(const AudioAsset&) = delete;

        bool Load(uint8_t* data, size_t len);

        bool Load(const std::string_view& path);

        void Unload();

        void AttachPlayer(AudioPlayer* player);

        void DetachPlayer(AudioPlayer* player);

        std::unique_ptr<IAudioDecoder> GetAudioDecoder();

        const std::string& GetError();
    };

} // Uniasset

#endif //UNIASSET_AUDIOASSET_HPP
