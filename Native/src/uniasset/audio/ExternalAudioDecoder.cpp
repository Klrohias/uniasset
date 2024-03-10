//
// Created by qingy on 2024/2/16.
//

#include "ExternalAudioDecoder.hpp"

namespace uniasset {
    ExternalAudioDecoder::ExternalAudioDecoder(void* userData,
                                               GetChannelCountFunc getChannelCountFunc,
                                               GetSampleCountFunc getSampleCountFunc,
                                               GetSampleFormatFunc getSampleFormatFunc,
                                               GetSampleRateFunc getSampleRateFunc,
                                               ReadFunc readFunc,
                                               SeekFunc seekFunc,
                                               TellFunc tellFunc)
            : userData_{userData},
              getChannelCountFunc_{getChannelCountFunc}, getSampleCountFunc_{getSampleCountFunc},
              getSampleFormatFunc_{getSampleFormatFunc}, getSampleRateFunc_{getSampleRateFunc}, readFunc_{readFunc},
              seekFunc_{seekFunc}, tellFunc_{tellFunc} {
    }

    uint32_t ExternalAudioDecoder::getChannelCount() {
        return getChannelCountFunc_(userData_);
    }

    size_t ExternalAudioDecoder::getSampleCount() {
        return getSampleCountFunc_(userData_);
    }

    SampleFormat ExternalAudioDecoder::getSampleFormat() {
        return getSampleFormatFunc_(userData_);
    }

    uint32_t ExternalAudioDecoder::getSampleRate() {
        return getSampleRateFunc_(userData_);
    }

    uint32_t ExternalAudioDecoder::read(void* buffer, uint32_t count) {
        return readFunc_(userData_, buffer, count);
    }

    bool ExternalAudioDecoder::seek(int64_t position) {
        return seekFunc_(userData_, position);
    }

    int64_t ExternalAudioDecoder::tell() {
        return tellFunc_(userData_);
    }
} // uniasset