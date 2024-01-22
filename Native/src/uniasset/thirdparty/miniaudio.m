//
// Created by Twiiz on 2024/1/22.
//
#ifdef __APPLE__
#include <TargetConditionals.h>

#ifdef TARGET_OS_IPHONE
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#endif

#endif