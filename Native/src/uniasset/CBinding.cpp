//
// Created by Twiiz on 2024/1/14.
//

#include "CBinding.h"

#include <memory>
#include "image/ImageAsset.hpp"
#include "audio/AudioPlayer.hpp"
#include "audio/AudioAsset.hpp"
#include "audio/IAudioDecoder.hpp"
#include "audio/FlacDecoder.hpp"
#include "audio/Mp3Decoder.hpp"
#include "audio/OggDecoder.hpp"
#include "audio/WavDecoder.hpp"
#include "audio/ExternalAudioDecoder.hpp"

using namespace uniasset;

template<typename T>
std::shared_ptr<T>* createInstance(T* item) {
    return new std::shared_ptr<T>(item);
}

template<typename T>
void destroyInstance(void* ref) {
    delete reinterpret_cast<std::shared_ptr<T>*>(ref);
}

template<typename T>
std::shared_ptr<T>& getInstance(void* ref) {
    return *reinterpret_cast<std::shared_ptr<T>*>(ref);
}

// ImageAsset

CBINDING_METHOD(CBINDING_TYPED_PTR(ImageAsset), ImageAsset, Create) {
    return createInstance<ImageAsset>(new ImageAsset);
}

CBINDING_METHOD(void, ImageAsset, Destory, CBINDING_TYPED_PTR(ImageAsset) obj) {
    destroyInstance<ImageAsset>(obj);
}

CBINDING_METHOD(void, ImageAsset, LoadFile, CBINDING_TYPED_PTR(ImageAsset) self, CBINDING_CSTRING path) {
    getInstance<ImageAsset>(self)->load(path);
}

CBINDING_METHOD(void, ImageAsset, LoadPixels, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* pixels,
                uint64_t size, int32_t width, int32_t height, int32_t channelCount) {
    getInstance<ImageAsset>(self)->load(pixels, size, width, height, channelCount);
}

CBINDING_METHOD(void, ImageAsset, Load, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* data,
                uint64_t size) {
    getInstance<ImageAsset>(self)->load(data, size);
}

CBINDING_METHOD(CBINDING_CSTRING, ImageAsset, GetError, CBINDING_TYPED_PTR(ImageAsset) self) {
    return getInstance<ImageAsset>(self)->getError().c_str();
}

CBINDING_METHOD(int32_t, ImageAsset, GetWidth, CBINDING_TYPED_PTR(ImageAsset) self) {
    return getInstance<ImageAsset>(self)->getWidth();
}

CBINDING_METHOD(int32_t, ImageAsset, GetHeight, CBINDING_TYPED_PTR(ImageAsset) self) {
    return getInstance<ImageAsset>(self)->getHeight();
}

CBINDING_METHOD(int32_t, ImageAsset, GetChannelCount, CBINDING_TYPED_PTR(ImageAsset) self) {
    return getInstance<ImageAsset>(self)->getChannelCount();
}

CBINDING_METHOD(void, ImageAsset, Clip, CBINDING_TYPED_PTR(ImageAsset) self, int32_t x, int32_t y,
                int32_t width, int32_t height) {
    getInstance<ImageAsset>(self)->clip(x, y, width, height);
}

CBINDING_METHOD(void, ImageAsset, Resize, CBINDING_TYPED_PTR(ImageAsset) self, int32_t width,
                int32_t height) {
    getInstance<ImageAsset>(self)->resize(width, height);
}

CBINDING_METHOD(void, ImageAsset, Unload, CBINDING_TYPED_PTR(ImageAsset) self) {
    getInstance<ImageAsset>(self)->unload();
}

CBINDING_METHOD(void, ImageAsset, CopyTo, CBINDING_TYPED_PTR(ImageAsset) self, void* dest) {
    getInstance<ImageAsset>(self)->copyTo(dest);
}

CBINDING_METHOD(CBINDING_TYPED_PTR(ImageAsset), ImageAsset, Clone, CBINDING_TYPED_PTR(ImageAsset) self) {
    return getInstance<ImageAsset>(self)->clone();
}

// AudioPlayer
CBINDING_METHOD(CBINDING_TYPED_PTR(AudioPlayer), AudioPlayer, Create) {
    return createInstance<AudioPlayer>(new AudioPlayer);
}

CBINDING_METHOD(void, AudioPlayer, Destory, CBINDING_TYPED_PTR(AudioPlayer) obj) {
    destroyInstance<AudioPlayer>(obj);
}

CBINDING_METHOD(CBINDING_CSTRING, AudioPlayer, GetError, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return getInstance<AudioPlayer>(self)->getError().c_str();
}

CBINDING_METHOD(void, AudioPlayer, Pause, CBINDING_TYPED_PTR(AudioPlayer) self) {
    getInstance<AudioPlayer>(self)->pause();
}

CBINDING_METHOD(void, AudioPlayer, Resume, CBINDING_TYPED_PTR(AudioPlayer) self) {
    getInstance<AudioPlayer>(self)->resume();
}

CBINDING_METHOD(void, AudioPlayer, Close, CBINDING_TYPED_PTR(AudioPlayer) self) {
    getInstance<AudioPlayer>(self)->close();
}

CBINDING_METHOD(void, AudioPlayer, Open, CBINDING_TYPED_PTR(AudioPlayer) self,
                CBINDING_TYPED_PTR(AudioAsset) audioAsset) {
    getInstance<AudioPlayer>(self)->open(getInstance<AudioAsset>(audioAsset));
}

CBINDING_METHOD(CBINDING_BOOLEAN, AudioPlayer, IsPaused, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return getInstance<AudioPlayer>(self)->isPaused();
}

CBINDING_METHOD(void, AudioPlayer, SetVolume, CBINDING_TYPED_PTR(AudioPlayer) self, float val) {
    getInstance<AudioPlayer>(self)->setVolume(val);
}

CBINDING_METHOD(float, AudioPlayer, GetVolume, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return getInstance<AudioPlayer>(self)->getVolume();
}

CBINDING_METHOD(float, AudioPlayer, GetTime, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return getInstance<AudioPlayer>(self)->getTime();
}

// AudioAsset
CBINDING_METHOD(CBINDING_TYPED_PTR(AudioAsset), AudioAsset, Create) {
    return createInstance<AudioAsset>(new AudioAsset);
}

CBINDING_METHOD(void, AudioAsset, Destory, CBINDING_TYPED_PTR(AudioAsset) obj) {
    destroyInstance<AudioAsset>(obj);
}

CBINDING_METHOD(CBINDING_CSTRING, AudioAsset, GetError, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getError().c_str();
}

CBINDING_METHOD(void, AudioAsset, LoadFile, CBINDING_TYPED_PTR(AudioAsset) self, CBINDING_CSTRING path) {
    getInstance<AudioAsset>(self)->load(path);
}

CBINDING_METHOD(void, AudioAsset, Load, CBINDING_TYPED_PTR(AudioAsset) self, uint8_t* data, uint64_t size) {
    getInstance<AudioAsset>(self)->load(std::span<uint8_t>(data, size));
}

CBINDING_METHOD(void, AudioAsset, Unload, CBINDING_TYPED_PTR(AudioAsset) self) {
    getInstance<AudioAsset>(self)->unload();
}

CBINDING_METHOD(uint32_t, AudioAsset, GetChannelCount, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getChannelCount();
}

CBINDING_METHOD(uint64_t, AudioAsset, GetSampleCount, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getSampleCount();
}

CBINDING_METHOD(uint32_t, AudioAsset, GetSampleRate, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getSampleRate();
}

CBINDING_METHOD(float, AudioAsset, GetLength, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getLength();
}

CBINDING_METHOD(uint8_t, AudioAsset, GetLoadType, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getLoadType();
}

CBINDING_METHOD(CBINDING_CSTRING, AudioAsset, GetPath, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getPath().c_str();
}

CBINDING_METHOD(void*, AudioAsset, GetData, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getData().get();
}

CBINDING_METHOD(uint64_t, AudioAsset, GetDataLength, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getDataLength();
}

CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), AudioAsset, GetAudioDecoder, CBINDING_TYPED_PTR(AudioAsset) self,
                uint8_t format, int64_t frameBufferSize) {
    return createInstance<IAudioDecoder>(getInstance<AudioAsset>(self)->getAudioDecoder(
            static_cast<SampleFormat>(format), frameBufferSize).release());
}

// IAudioDecoder
CBINDING_METHOD(void, IAudioDecoder, Destory, CBINDING_TYPED_PTR(IAudioDecoder) obj) {
    destroyInstance<IAudioDecoder>(obj);
}

CBINDING_METHOD(uint8_t, IAudioDecoder, GetSampleFormat, CBINDING_TYPED_PTR(IAudioDecoder) self) {
    return getInstance<IAudioDecoder>(self)->getSampleFormat();
}

CBINDING_METHOD(uint32_t, IAudioDecoder, GetChannelCount, CBINDING_TYPED_PTR(IAudioDecoder) self) {
    return getInstance<IAudioDecoder>(self)->getChannelCount();
}

CBINDING_METHOD(uint64_t, IAudioDecoder, GetSampleCount, CBINDING_TYPED_PTR(IAudioDecoder) self) {
    return getInstance<IAudioDecoder>(self)->getSampleCount();
}

CBINDING_METHOD(uint32_t, IAudioDecoder, GetSampleRate, CBINDING_TYPED_PTR(IAudioDecoder) self) {
    return getInstance<IAudioDecoder>(self)->getSampleRate();
}

CBINDING_METHOD(CBINDING_BOOLEAN, IAudioDecoder, Read, CBINDING_TYPED_PTR(IAudioDecoder) self, void* buffer,
                uint32_t count) {
    return getInstance<IAudioDecoder>(self)->read(buffer, count);
}

CBINDING_METHOD(CBINDING_BOOLEAN, IAudioDecoder, Seek, CBINDING_TYPED_PTR(IAudioDecoder) self, int64_t pos) {
    return getInstance<IAudioDecoder>(self)->seek(pos);
}

CBINDING_METHOD(int64_t, IAudioDecoder, Tell, CBINDING_TYPED_PTR(IAudioDecoder) self) {
    return getInstance<IAudioDecoder>(self)->tell();
}

// FlacDecoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), FlacDecoder, Create, CBINDING_TYPED_PTR(AudioAsset) asset,
                uint8_t format) {
    return createInstance<IAudioDecoder>(new FlacDecoder(getInstance<AudioAsset>(asset),
                                                         static_cast<SampleFormat>(format)));
}

// OggDecoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), OggDecoder, Create, CBINDING_TYPED_PTR(AudioAsset) asset,
                uint8_t format) {
    return createInstance<IAudioDecoder>(new OggDecoder(getInstance<AudioAsset>(asset),
                                                        static_cast<SampleFormat>(format)));
}

// WavDecoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), WavDecoder, Create, CBINDING_TYPED_PTR(AudioAsset) asset,
                uint8_t format) {
    return createInstance<IAudioDecoder>(new WavDecoder(getInstance<AudioAsset>(asset),
                                                        static_cast<SampleFormat>(format)));
}

// Mp3Decoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), Mp3Decoder, Create, CBINDING_TYPED_PTR(AudioAsset) asset,
                uint8_t format) {
    return createInstance<IAudioDecoder>(new Mp3Decoder(getInstance<AudioAsset>(asset),
                                                        static_cast<SampleFormat>(format)));
}

// ExternalAudioDecoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), ExternalAudioDecoder, Create, void* userData,
                void* getChannelCountFunc,
                void* getSampleCountFunc,
                void* getSampleFormatFunc,
                void* getSampleRateFunc,
                void* readFunc) {
    return createInstance<IAudioDecoder>(
            new ExternalAudioDecoder(userData,
                                     reinterpret_cast<ExternalAudioDecoder::GetChannelCountFunc>(getChannelCountFunc),
                                     reinterpret_cast<ExternalAudioDecoder::GetSampleCountFunc>(getSampleCountFunc),
                                     reinterpret_cast<ExternalAudioDecoder::GetSampleFormatFunc>(getSampleFormatFunc),
                                     reinterpret_cast<ExternalAudioDecoder::GetSampleRateFunc>(getSampleRateFunc),
                                     reinterpret_cast<ExternalAudioDecoder::ReadFunc>(readFunc)));
}
