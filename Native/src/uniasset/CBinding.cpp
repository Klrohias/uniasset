//
// Created by Twiiz on 2024/1/14.
//

#include "CBinding.h"

#include <memory>
#include "image/ImageAsset.hpp"
#include "audio/AudioPlayer.hpp"
#include "audio/AudioAsset.hpp"

using namespace uniasset;

template<typename T>
std::shared_ptr<T>* createInstance(T* item) {
    return new std::shared_ptr<T>(item->shared_from_this());
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

CBINDING_METHOD(void, ImageAsset, Free, CBINDING_TYPED_PTR(ImageAsset) obj) {
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

CBINDING_METHOD(void, AudioPlayer, Free, CBINDING_TYPED_PTR(AudioPlayer) obj) {
    destroyInstance<AudioPlayer>(obj);
}

CBINDING_METHOD(CBINDING_CSTRING, AudioPlayer, GetError, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return getInstance<AudioPlayer>(self)->GetError().c_str();
}

CBINDING_METHOD(void, AudioPlayer, Pause, CBINDING_TYPED_PTR(AudioPlayer) self) {
    getInstance<AudioPlayer>(self)->Pause();
}

CBINDING_METHOD(void, AudioPlayer, Resume, CBINDING_TYPED_PTR(AudioPlayer) self) {
    getInstance<AudioPlayer>(self)->Resume();
}

CBINDING_METHOD(void, AudioPlayer, Close, CBINDING_TYPED_PTR(AudioPlayer) self) {
    getInstance<AudioPlayer>(self)->Close();
}

CBINDING_METHOD(void, AudioPlayer, Open, CBINDING_TYPED_PTR(AudioPlayer) self,
                CBINDING_TYPED_PTR(AudioAsset) audioAsset) {
    getInstance<AudioPlayer>(self)->Open(reinterpret_cast<AudioAsset*>(audioAsset));
}

CBINDING_METHOD(CBINDING_BOOLEAN, AudioPlayer, IsPaused, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return getInstance<AudioPlayer>(self)->IsPaused();
}

CBINDING_METHOD(void, AudioPlayer, SetVolume, CBINDING_TYPED_PTR(AudioPlayer) self, float val) {
    getInstance<AudioPlayer>(self)->SetVolume(val);
}

CBINDING_METHOD(float, AudioPlayer, GetVolume, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return getInstance<AudioPlayer>(self)->GetVolume();
}

CBINDING_METHOD(float, AudioPlayer, GetTime, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return getInstance<AudioPlayer>(self)->GetTime();
}

// AudioAsset
CBINDING_METHOD(CBINDING_TYPED_PTR(AudioAsset), AudioAsset, Create) {
    return createInstance<AudioAsset>(new AudioAsset);
}

CBINDING_METHOD(void, AudioAsset, Free, CBINDING_TYPED_PTR(AudioAsset) obj) {
    destroyInstance<AudioAsset>(obj);
}

CBINDING_METHOD(CBINDING_CSTRING, AudioAsset, GetError, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getError().c_str();
}

CBINDING_METHOD(void, AudioAsset, LoadFile, CBINDING_TYPED_PTR(AudioAsset) self, CBINDING_CSTRING path) {
    getInstance<AudioAsset>(self)->load(path);
}

CBINDING_METHOD(void, AudioAsset, load, CBINDING_TYPED_PTR(AudioAsset) self, uint8_t* data, uint64_t size) {
    getInstance<AudioAsset>(self)->load(std::span<uint8_t>(data, size));
}

CBINDING_METHOD(void, AudioAsset, unload, CBINDING_TYPED_PTR(AudioAsset) self) {
    getInstance<AudioAsset>(self)->unload();
}

CBINDING_METHOD(uint32_t, AudioAsset, GetChannelCount, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getChannelCount();
}

CBINDING_METHOD(uint64_t, AudioAsset, getSampleCount, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getSampleCount();
}

CBINDING_METHOD(uint32_t, AudioAsset, getSampleRate, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getSampleRate();
}

CBINDING_METHOD(float, AudioAsset, GetLength, CBINDING_TYPED_PTR(AudioAsset) self) {
    return getInstance<AudioAsset>(self)->getLength();
}
