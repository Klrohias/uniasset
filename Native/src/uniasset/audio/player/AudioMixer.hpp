#pragma once
#ifndef UNIASSET_AUDIOMIXER_H
#define UNIASSET_AUDIOMIXER_H

#include <cstdint>
#include <limits>
#include <vector>
#include <algorithm>

#include "uniasset/thirdparty/miniaudio.hpp"
#include "uniasset/audio/player/IWaveProvider.hpp"

namespace uniasset {
    struct AudioMixer {
    public:
        typedef void (*mixSampleEndFunc)(AudioMixer* pMixer);
        typedef void (*mixSampleFunc)(AudioMixer* pMixer, const IWaveProvider::ReadResult& waveReadResult);

        void* pDst_{ nullptr };
        uint32_t frameCount_{ 0 };
        uint32_t uncleanFrameCount_{ 0 };

        uint32_t bytesPerFrame_{ 0 };
        uint32_t bytesPerSample_{ 0 };
        uint32_t channelCount_{ 0 };
        
        void* pBuffer{ nullptr };
        size_t bufferSize_{ 0 };
        bool isBufferUsed_{ true };

        mixSampleEndFunc mixSampleEnd{ nullptr };
        mixSampleFunc mixSample{ nullptr };

        AudioMixer();

        AudioMixer(ma_format waveFormat, uint32_t channel);

        ~AudioMixer();

        void begin(void* pDst, uint32_t frameCount);

        void mix(const IWaveProvider::ReadResult& waveReadResult);

        void end();

        void ensureBufferCapacity(size_t size);
    };
}
#endif // !UNIASSET_AUDIOMIXER_H
