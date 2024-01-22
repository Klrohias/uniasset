//
// Created by Twiiz on 2024/1/14.
//

#include "CBinding.h"

#include "image/ImageAsset.hpp"
#include "audio/AudioPlayer.hpp"
#include "audio/AudioAsset.hpp"

using namespace Uniasset;

// ImageAsset

CBINDING_METHOD(CBINDING_TYPED_PTR(ImageAsset), ImageAsset, Create) {
    return new ImageAsset;
}

CBINDING_METHOD(void, ImageAsset, Free, CBINDING_TYPED_PTR(ImageAsset) obj) {
    delete reinterpret_cast<ImageAsset*>(obj);
}

CBINDING_METHOD(void, ImageAsset, LoadFile, CBINDING_TYPED_PTR(ImageAsset) self, CBINDING_CSTRING path) {
    reinterpret_cast<ImageAsset*>(self)->Load(path);
}

CBINDING_METHOD(void, ImageAsset, LoadPixels, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* pixels,
                uint64_t size, int32_t width, int32_t height, int32_t channelCount) {
    reinterpret_cast<ImageAsset*>(self)->Load(pixels, size, width, height, channelCount);
}

CBINDING_METHOD(void, ImageAsset, Load, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* data,
                uint64_t size) {
    reinterpret_cast<ImageAsset*>(self)->Load(data, size);
}

CBINDING_METHOD(CBINDING_CSTRING, ImageAsset, GetError, CBINDING_TYPED_PTR(ImageAsset) self) {
    return reinterpret_cast<ImageAsset*>(self)->GetError().c_str();
}

CBINDING_METHOD(int32_t, ImageAsset, GetWidth, CBINDING_TYPED_PTR(ImageAsset) self) {
    return reinterpret_cast<ImageAsset*>(self)->GetWidth();
}

CBINDING_METHOD(int32_t, ImageAsset, GetHeight, CBINDING_TYPED_PTR(ImageAsset) self) {
    return reinterpret_cast<ImageAsset*>(self)->GetHeight();
}

CBINDING_METHOD(int32_t, ImageAsset, GetChannelCount, CBINDING_TYPED_PTR(ImageAsset) self) {
    return reinterpret_cast<ImageAsset*>(self)->GetChannelCount();
}

CBINDING_METHOD(void, ImageAsset, Clip, CBINDING_TYPED_PTR(ImageAsset) self, int32_t x, int32_t y,
                int32_t width, int32_t height) {
    reinterpret_cast<ImageAsset*>(self)->Clip(x, y, width, height);
}

CBINDING_METHOD(void, ImageAsset, Resize, CBINDING_TYPED_PTR(ImageAsset) self, int32_t width,
                int32_t height) {
    reinterpret_cast<ImageAsset*>(self)->Resize(width, height);
}

CBINDING_METHOD(void, ImageAsset, Unload, CBINDING_TYPED_PTR(ImageAsset) self) {
    reinterpret_cast<ImageAsset*>(self)->Unload();
}

CBINDING_METHOD(void, ImageAsset, CopyTo, CBINDING_TYPED_PTR(ImageAsset) self, void* dest) {
    reinterpret_cast<ImageAsset*>(self)->CopyTo(dest);
}

CBINDING_METHOD(CBINDING_TYPED_PTR(ImageAsset), ImageAsset, Clone, CBINDING_TYPED_PTR(ImageAsset) self) {
    return reinterpret_cast<ImageAsset*>(self)->Clone();
}


// AudioPlayer
CBINDING_METHOD(CBINDING_TYPED_PTR(AudioPlayer), AudioPlayer, Create) {
    return new AudioPlayer;
}

CBINDING_METHOD(void, AudioPlayer, Free, CBINDING_TYPED_PTR(AudioPlayer) obj) {
    delete reinterpret_cast<AudioPlayer*>(obj);
}

CBINDING_METHOD(CBINDING_CSTRING, AudioPlayer, GetError, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return reinterpret_cast<AudioPlayer*>(self)->GetError().c_str();
}

CBINDING_METHOD(void, AudioPlayer, Pause, CBINDING_TYPED_PTR(AudioPlayer) self) {
    reinterpret_cast<AudioPlayer*>(self)->Pause();
}

CBINDING_METHOD(void, AudioPlayer, Resume, CBINDING_TYPED_PTR(AudioPlayer) self) {
    reinterpret_cast<AudioPlayer*>(self)->Resume();
}

CBINDING_METHOD(void, AudioPlayer, Close, CBINDING_TYPED_PTR(AudioPlayer) self) {
    reinterpret_cast<AudioPlayer*>(self)->Close();
}

CBINDING_METHOD(void, AudioPlayer, Open, CBINDING_TYPED_PTR(AudioPlayer) self,
                CBINDING_TYPED_PTR(AudioAsset) audioAsset) {
    reinterpret_cast<AudioPlayer*>(self)->Open(reinterpret_cast<AudioAsset*>(audioAsset));
}

CBINDING_METHOD(CBINDING_BOOLEAN, AudioPlayer, IsPaused, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return reinterpret_cast<AudioPlayer*>(self)->IsPaused();
}

CBINDING_METHOD(void, AudioPlayer, SetVolume, CBINDING_TYPED_PTR(AudioPlayer) self, float val) {
    reinterpret_cast<AudioPlayer*>(self)->SetVolume(val);
}

CBINDING_METHOD(float, AudioPlayer, GetVolume, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return reinterpret_cast<AudioPlayer*>(self)->GetVolume();
}

CBINDING_METHOD(float, AudioPlayer, GetTime, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return reinterpret_cast<AudioPlayer*>(self)->GetTime();
}

// AudioAsset
CBINDING_METHOD(CBINDING_TYPED_PTR(AudioAsset), AudioAsset, Create) {
    return new AudioAsset;
}

CBINDING_METHOD(void, AudioAsset, Free, CBINDING_TYPED_PTR(AudioAsset) obj) {
    delete reinterpret_cast<AudioAsset*>(obj);
}

CBINDING_METHOD(CBINDING_CSTRING, AudioAsset, GetError, CBINDING_TYPED_PTR(AudioAsset) self) {
    return reinterpret_cast<AudioAsset*>(self)->GetError().c_str();
}

CBINDING_METHOD(void, AudioAsset, LoadFile, CBINDING_TYPED_PTR(AudioAsset) self, CBINDING_CSTRING path) {
    reinterpret_cast<AudioAsset*>(self)->Load(path);
}

CBINDING_METHOD(void, AudioAsset, Load, CBINDING_TYPED_PTR(AudioAsset) self, uint8_t* data, uint64_t size) {
    reinterpret_cast<AudioAsset*>(self)->Load(data, size);
}

CBINDING_METHOD(void, AudioAsset, Unload, CBINDING_TYPED_PTR(AudioAsset) self) {
    reinterpret_cast<AudioAsset*>(self)->Unload();
}

CBINDING_METHOD(uint32_t, AudioAsset, GetChannelCount, CBINDING_TYPED_PTR(AudioAsset) self) {
    return reinterpret_cast<AudioAsset*>(self)->GetChannelCount();
}

CBINDING_METHOD(uint64_t, AudioAsset, GetSampleCount, CBINDING_TYPED_PTR(AudioAsset) self) {
    return reinterpret_cast<AudioAsset*>(self)->GetSampleCount();
}

CBINDING_METHOD(uint32_t, AudioAsset, GetSampleRate, CBINDING_TYPED_PTR(AudioAsset) self) {
    return reinterpret_cast<AudioAsset*>(self)->GetSampleRate();
}

CBINDING_METHOD(float, AudioAsset, GetLength, CBINDING_TYPED_PTR(AudioAsset) self) {
    return reinterpret_cast<AudioAsset*>(self)->GetLength();
}
