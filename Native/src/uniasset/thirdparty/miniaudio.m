//
// Created by Twiiz on 2024/1/22.
//
#ifdef __APPLE__
#include <TargetConditionals.h>

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#endif

#endif