//
// Created by Twiiz on 2024/1/22.
//
#ifdef __APPLE__
#include <TargetConditionals.h>

#ifdef TARGET_OS_IPHONE

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#endif

#endif