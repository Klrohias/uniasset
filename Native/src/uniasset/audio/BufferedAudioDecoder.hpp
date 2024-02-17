//
// Created by qingy on 2024/2/16.
//

#pragma once
#ifndef UNIASSET_BUFFEREDAUDIODECODER_HPP
#define UNIASSET_BUFFEREDAUDIODECODER_HPP

#include <memory>
#include <mutex>
#include "uniasset/audio/IAudioDecoder.hpp"

namespace uniasset {

    class BufferedAudioDecoder : public IAudioDecoder {
        std::shared_ptr<IAudioDecoder> wrappedAudioDecoder_{};

        std::unique_ptr<uint8_t[]> buffer_{};

        std::unique_ptr<std::mutex> sync_lock{};

        // unit: frames
        int64_t bufferOffset_{0};
        // unit: frames
        int64_t bufferSize_{0};
        // unit: frames
        int64_t bufferedFrameBegin_{0};
        // unit: frames
        int64_t bufferedFrameEnd_{0};

        int64_t frameSize_{0};

        void generateBuffer();

        void generateBufferImmediately();

        void readInternal(void* buffer, int64_t requiredFrame);

    public:
        explicit BufferedAudioDecoder(std::shared_ptr<IAudioDecoder> wrappedAudioDecoder, int64_t bufferFrames = -1);

        BufferedAudioDecoder(BufferedAudioDecoder&&) = default;

        ~BufferedAudioDecoder() override = default;

        int64_t tell() override;

        bool seek(int64_t position) override;

        uint32_t getSampleRate() override;

        SampleFormat getSampleFormat() override;

        size_t getSampleCount() override;

        uint32_t getChannelCount() override;

        bool read(void* buffer, uint32_t count) override;
    };

} // uniasset

#endif //UNIASSET_BUFFEREDAUDIODECODER_HPP
