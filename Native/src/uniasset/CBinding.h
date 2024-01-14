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

CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioPlayer), IAudioPlayer, Create);
CBINDING_METHOD(void, IAudioPlayer, Free, CBINDING_TYPED_PTR(IAudioPlayer) player);

// ImageAsset
CBINDING_METHOD(CBINDING_TYPED_PTR(ImageAsset), ImageAsset, Create);
CBINDING_METHOD(void, ImageAsset, Free, CBINDING_TYPED_PTR(ImageAsset) obj);
CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, LoadFile, CBINDING_TYPED_PTR(ImageAsset) self, CBINDING_CSTRING path);
CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, LoadPixels, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* pixels,
                uint64_t size, int32_t width, int32_t height, int32_t channelCount);
CBINDING_METHOD(CBINDING_BOOLEAN, ImageAsset, Load, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* data,
                uint64_t size);
CBINDING_METHOD(CBINDING_CSTRING, ImageAsset, GetError, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(int32_t, ImageAsset, GetWidth, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(int32_t, ImageAsset, GetHeight, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(int32_t, ImageAsset, GetChannelCount, CBINDING_TYPED_PTR(ImageAsset) self);

#ifdef __cplusplus
}
#endif

#endif //UNIASSET_CBINDING_H
