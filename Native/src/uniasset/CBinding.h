//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_CBINDING_H
#define UNIASSET_CBINDING_H

#include "Foundation.hpp"

#ifdef __cplusplus

#include <cstdint>

#else
#include "stdint.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ImageAsset
CBINDING_METHOD(CBINDING_TYPED_PTR(ImageAsset), ImageAsset, Create);
CBINDING_METHOD(void, ImageAsset, Free, CBINDING_TYPED_PTR(ImageAsset) obj);
CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, LoadFile, CBINDING_TYPED_PTR(ImageAsset) self, CBINDING_CSTRING path);
CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, LoadPixels, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* pixels,
                uint64_t size, int32_t width, int32_t height, int32_t channelCount);
CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, Load, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* data, uint64_t size);
CBINDING_METHOD(CBINDING_CSTRING, ImageAsset, GetError, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(int32_t, ImageAsset, GetWidth, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(int32_t, ImageAsset, GetHeight, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(int32_t, ImageAsset, GetChannelCount, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, Clip, CBINDING_TYPED_PTR(ImageAsset) self, int32_t x, int32_t y,
                int32_t width, int32_t height);
CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, Resize, CBINDING_TYPED_PTR(ImageAsset) self, int32_t width,
                int32_t height);
CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, Unload, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, CopyTo, CBINDING_TYPED_PTR(ImageAsset) self, void* dest);
CBINDING_METHOD(CBINDING_TYPED_PTR(ImageAsset), ImageAsset, Clone, CBINDING_TYPED_PTR(ImageAsset) self);

// AudioPlayer
CBINDING_METHOD(CBINDING_TYPED_PTR(AudioPlayer), AudioPlayer, Create);
CBINDING_METHOD(void, AudioPlayer, Free, CBINDING_TYPED_PTR(AudioPlayer) obj);
CBINDING_METHOD(CBINDING_CSTRING, AudioPlayer, GetError, CBINDING_TYPED_PTR(AudioPlayer) self);
CBINDING_METHOD(void, AudioPlayer, Pause, CBINDING_TYPED_PTR(AudioPlayer) self);
CBINDING_METHOD(void, AudioPlayer, Resume, CBINDING_TYPED_PTR(AudioPlayer) self);
CBINDING_METHOD(void, AudioPlayer, Close, CBINDING_TYPED_PTR(AudioPlayer) self);
CBINDING_METHOD(CBINDING_BOOLEAN, AudioPlayer, Open, CBINDING_TYPED_PTR(AudioPlayer) self,
                CBINDING_TYPED_PTR(AudioAsset) audioAsset);
CBINDING_METHOD(CBINDING_BOOLEAN, AudioPlayer, IsPaused, CBINDING_TYPED_PTR(AudioPlayer) self);
CBINDING_METHOD(void, AudioPlayer, SetVolume, CBINDING_TYPED_PTR(AudioPlayer) self, float val);
CBINDING_METHOD(float, AudioPlayer, GetVolume, CBINDING_TYPED_PTR(AudioPlayer) self);

// AudioAsset
CBINDING_METHOD(CBINDING_TYPED_PTR(AudioAsset), AudioAsset, Create);
CBINDING_METHOD(void, AudioAsset, Free, CBINDING_TYPED_PTR(AudioAsset) obj);
CBINDING_METHOD(CBINDING_CSTRING, AudioAsset, GetError, CBINDING_TYPED_PTR(AudioAsset) self);
CBINDING_METHOD(CBINDING_BOOLEAN, AudioAsset, LoadFile, CBINDING_TYPED_PTR(AudioAsset) self, CBINDING_CSTRING path);
CBINDING_METHOD(CBINDING_BOOLEAN, AudioAsset, Load, CBINDING_TYPED_PTR(AudioAsset) self, uint8_t* data, uint64_t size);
CBINDING_METHOD(void, AudioAsset, Unload, CBINDING_TYPED_PTR(AudioAsset) self);

#ifdef __cplusplus
}
#endif

#endif //UNIASSET_CBINDING_H
