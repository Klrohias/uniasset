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
CBINDING_METHOD(void, ImageAsset, Destory, CBINDING_TYPED_PTR(ImageAsset) obj);
CBINDING_METHOD(void, ImageAsset, LoadFile, CBINDING_TYPED_PTR(ImageAsset) self, CBINDING_CSTRING path);
CBINDING_METHOD(void, ImageAsset, LoadEmpty, CBINDING_TYPED_PTR(ImageAsset) self, int32_t width, int32_t height,
                int32_t channelCount);
CBINDING_METHOD(void, ImageAsset, LoadPixels, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* pixels,
                uint64_t size, int32_t width, int32_t height, int32_t channelCount);
CBINDING_METHOD(void, ImageAsset, Load, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* data, uint64_t size);
CBINDING_METHOD(CBINDING_CSTRING, ImageAsset, GetError, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(int32_t, ImageAsset, GetWidth, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(int32_t, ImageAsset, GetHeight, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(int32_t, ImageAsset, GetChannelCount, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(void, ImageAsset, Crop, CBINDING_TYPED_PTR(ImageAsset) self, int32_t x, int32_t y,
                int32_t width, int32_t height);
CBINDING_METHOD(void, ImageAsset, Resize, CBINDING_TYPED_PTR(ImageAsset) self, int32_t width,
                int32_t height);
CBINDING_METHOD(void, ImageAsset, Unload, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(void, ImageAsset, CopyTo, CBINDING_TYPED_PTR(ImageAsset) self, void* dest);
CBINDING_METHOD(CBINDING_TYPED_PTR(ImageAsset), ImageAsset, Clone, CBINDING_TYPED_PTR(ImageAsset) self);
CBINDING_METHOD(void, ImageAsset, CropMultiple, CBINDING_TYPED_PTR(ImageAsset) self, void* items, int16_t count,
                void** output);

// AudioPlayer
CBINDING_METHOD(CBINDING_TYPED_PTR(AudioPlayer), AudioPlayer, Create);
CBINDING_METHOD(void, AudioPlayer, Destory, CBINDING_TYPED_PTR(AudioPlayer) obj);
CBINDING_METHOD(CBINDING_CSTRING, AudioPlayer, GetError, CBINDING_TYPED_PTR(AudioPlayer) self);
CBINDING_METHOD(void, AudioPlayer, Pause, CBINDING_TYPED_PTR(AudioPlayer) self);
CBINDING_METHOD(void, AudioPlayer, Resume, CBINDING_TYPED_PTR(AudioPlayer) self);
CBINDING_METHOD(void, AudioPlayer, Close, CBINDING_TYPED_PTR(AudioPlayer) self);
CBINDING_METHOD(void, AudioPlayer, Open, CBINDING_TYPED_PTR(AudioPlayer) self,
                CBINDING_TYPED_PTR(AudioAsset) audioAsset);
CBINDING_METHOD(CBINDING_BOOLEAN, AudioPlayer, IsPaused, CBINDING_TYPED_PTR(AudioPlayer) self);
CBINDING_METHOD(void, AudioPlayer, SetVolume, CBINDING_TYPED_PTR(AudioPlayer) self, float val);
CBINDING_METHOD(float, AudioPlayer, GetVolume, CBINDING_TYPED_PTR(AudioPlayer) self);
CBINDING_METHOD(float, AudioPlayer, GetTime, CBINDING_TYPED_PTR(AudioPlayer) self);
CBINDING_METHOD(void, AudioPlayer, SetTime, CBINDING_TYPED_PTR(AudioPlayer) self, float time);

// AudioAsset
CBINDING_METHOD(CBINDING_TYPED_PTR(AudioAsset), AudioAsset, Create);
CBINDING_METHOD(void, AudioAsset, Destory, CBINDING_TYPED_PTR(AudioAsset) obj);
CBINDING_METHOD(CBINDING_CSTRING, AudioAsset, GetError, CBINDING_TYPED_PTR(AudioAsset) self);
CBINDING_METHOD(void, AudioAsset, LoadFile, CBINDING_TYPED_PTR(AudioAsset) self, CBINDING_CSTRING path);
CBINDING_METHOD(void, AudioAsset, Load, CBINDING_TYPED_PTR(AudioAsset) self, uint8_t* data, uint64_t size);
CBINDING_METHOD(void, AudioAsset, Unload, CBINDING_TYPED_PTR(AudioAsset) self);
CBINDING_METHOD(uint32_t, AudioAsset, GetChannelCount, CBINDING_TYPED_PTR(AudioAsset) self);
CBINDING_METHOD(uint64_t, AudioAsset, GetSampleCount, CBINDING_TYPED_PTR(AudioAsset) self);
CBINDING_METHOD(uint32_t, AudioAsset, GetSampleRate, CBINDING_TYPED_PTR(AudioAsset) self);
CBINDING_METHOD(float, AudioAsset, GetLength, CBINDING_TYPED_PTR(AudioAsset) self);
CBINDING_METHOD(uint8_t, AudioAsset, GetLoadType, CBINDING_TYPED_PTR(AudioAsset) self);
CBINDING_METHOD(CBINDING_CSTRING, AudioAsset, GetPath, CBINDING_TYPED_PTR(AudioAsset) self);
CBINDING_METHOD(void*, AudioAsset, GetData, CBINDING_TYPED_PTR(AudioAsset) self);
CBINDING_METHOD(uint64_t, AudioAsset, GetDataLength, CBINDING_TYPED_PTR(AudioAsset) self);
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), AudioAsset, GetAudioDecoder, CBINDING_TYPED_PTR(AudioAsset) self,
                uint8_t format, int64_t frameBufferSize);

// IAudioDecoder
CBINDING_METHOD(void, IAudioDecoder, Destory, CBINDING_TYPED_PTR(IAudioDecoder) obj);
CBINDING_METHOD(uint8_t, IAudioDecoder, GetSampleFormat, CBINDING_TYPED_PTR(IAudioDecoder) self);
CBINDING_METHOD(uint32_t, IAudioDecoder, GetChannelCount, CBINDING_TYPED_PTR(IAudioDecoder) self);
CBINDING_METHOD(uint64_t, IAudioDecoder, GetSampleCount, CBINDING_TYPED_PTR(IAudioDecoder) self);
CBINDING_METHOD(uint32_t, IAudioDecoder, GetSampleRate, CBINDING_TYPED_PTR(IAudioDecoder) self);
CBINDING_METHOD(uint32_t, IAudioDecoder, Read, CBINDING_TYPED_PTR(IAudioDecoder) self, void* buffer,
                uint32_t count);
CBINDING_METHOD(CBINDING_BOOLEAN, IAudioDecoder, Seek, CBINDING_TYPED_PTR(IAudioDecoder) self, int64_t pos);
CBINDING_METHOD(int64_t, IAudioDecoder, Tell, CBINDING_TYPED_PTR(IAudioDecoder) self);

// FlacDecoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), FlacDecoder, Create, CBINDING_TYPED_PTR(AudioAsset) asset,
                uint8_t format);

// OggDecoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), OggDecoder, Create, CBINDING_TYPED_PTR(AudioAsset) asset,
                uint8_t format);

// WavDecoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), WavDecoder, Create, CBINDING_TYPED_PTR(AudioAsset) asset,
                uint8_t format);

// Mp3Decoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), Mp3Decoder, Create, CBINDING_TYPED_PTR(AudioAsset) asset,
                uint8_t format);

#ifdef __cplusplus
}
#endif

#endif //UNIASSET_CBINDING_H
