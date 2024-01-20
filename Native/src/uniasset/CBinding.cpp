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

CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, LoadFile, CBINDING_TYPED_PTR(ImageAsset) self, CBINDING_CSTRING path) {
    return reinterpret_cast<ImageAsset*>(self)->Load(path);
}

CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, LoadPixels, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* pixels,
                uint64_t size, int32_t width, int32_t height, int32_t channelCount) {
    return reinterpret_cast<ImageAsset*>(self)->Load(pixels, size, width, height, channelCount);
}

CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, Load, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* data,
                uint64_t size) {
    return reinterpret_cast<ImageAsset*>(self)->Load(data, size);
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

CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, Clip, CBINDING_TYPED_PTR(ImageAsset) self, int32_t x, int32_t y,
                int32_t width, int32_t height) {
    return reinterpret_cast<ImageAsset*>(self)->Clip(x, y, width, height);
}

CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, Resize, CBINDING_TYPED_PTR(ImageAsset) self, int32_t width,
                int32_t height) {
    return reinterpret_cast<ImageAsset*>(self)->Resize(width, height);
}

CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, Unload, CBINDING_TYPED_PTR(ImageAsset) self) {
    return reinterpret_cast<ImageAsset*>(self)->Unload();
}

CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, CopyTo, CBINDING_TYPED_PTR(ImageAsset) self, void* dest) {
    return reinterpret_cast<ImageAsset*>(self)->CopyTo(dest);
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

CBINDING_METHOD(CBINDING_BOOLEAN, AudioPlayer, Pause, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return reinterpret_cast<AudioPlayer*>(self)->Pause();
}

CBINDING_METHOD(CBINDING_BOOLEAN, AudioPlayer, Resume, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return reinterpret_cast<AudioPlayer*>(self)->Resume();
}

CBINDING_METHOD(CBINDING_BOOLEAN, AudioPlayer, Close, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return reinterpret_cast<AudioPlayer*>(self)->Close();
}

CBINDING_METHOD(CBINDING_BOOLEAN, AudioPlayer, Open, CBINDING_TYPED_PTR(AudioPlayer) self,
                CBINDING_TYPED_PTR(AudioAsset) audioAsset) {
    return reinterpret_cast<AudioPlayer*>(self)->Open(reinterpret_cast<AudioAsset*>(audioAsset));
}

CBINDING_METHOD(CBINDING_BOOLEAN, AudioPlayer, IsPaused, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return reinterpret_cast<AudioPlayer*>(self)->IsPaused();
}

CBINDING_METHOD(CBINDING_BOOLEAN, AudioPlayer, SetVolume, CBINDING_TYPED_PTR(AudioPlayer) self, float val) {
    return reinterpret_cast<AudioPlayer*>(self)->SetVolume(val);
}

CBINDING_METHOD(float, AudioPlayer, GetVolume, CBINDING_TYPED_PTR(AudioPlayer) self) {
    return reinterpret_cast<AudioPlayer*>(self)->GetVolume();
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

