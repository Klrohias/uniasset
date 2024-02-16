//
// Created by qingy on 2024/2/16.
//

#ifndef UNIASSET_EXTERNALAUDIODECODER_HPP
#define UNIASSET_EXTERNALAUDIODECODER_HPP

#include <cstdint>

#include "uniasset/audio/SampleFormat.hpp"
#include "uniasset/audio/IAudioDecoder.hpp"

namespace uniasset {
    class ExternalAudioDecoder : public IAudioDecoder {
    public:
        using GetChannelCountFunc = uint32_t(*)(void*);
        using GetSampleCountFunc = size_t(*)(void*);
        using GetSampleFormatFunc = SampleFormat(*)(void*);
        using GetSampleRateFunc = uint32_t(*)(void*);
        using ReadFunc = uint8_t(*)(void*, void*, uint32_t);

    private:
        void* userData_;
        GetChannelCountFunc getChannelCountFunc_{};
        GetSampleCountFunc getSampleCountFunc_{};
        GetSampleFormatFunc getSampleFormatFunc_{};
        GetSampleRateFunc getSampleRateFunc_{};
        ReadFunc readFunc_{};

    public:

        explicit ExternalAudioDecoder(void* userData,
                                      GetChannelCountFunc getChannelCountFunc,
                                      GetSampleCountFunc getSampleCountFunc,
                                      GetSampleFormatFunc getSampleFormatFunc,
                                      GetSampleRateFunc getSampleRateFunc,
                                      ReadFunc readFunc);

        uint32_t getChannelCount() override;

        size_t getSampleCount() override;

        SampleFormat getSampleFormat() override;

        uint32_t getSampleRate() override;

        bool read(void* buffer, uint32_t count) override;
    };

} // uniasset

#endif //UNIASSET_EXTERNALAUDIODECODER_HPP
