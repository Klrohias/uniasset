//
// Created by Twiiz on 2024/1/14.
//

#include "CBinding.h"

#include "common/IAudioPlayer.hpp"
#include "image/ImageAsset.hpp"

using namespace Uniasset;

CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioPlayer), IAudioPlayer, Create) {
    return nullptr;
}

CBINDING_METHOD(void, IAudioPlayer, Free, CBINDING_TYPED_PTR(IAudioPlayer) player) {
    delete reinterpret_cast<IAudioPlayer*>(player);
}


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

