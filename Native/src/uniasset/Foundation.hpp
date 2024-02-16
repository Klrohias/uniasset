//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_LIBRARY_HPP
#define UNIASSET_LIBRARY_HPP

#if (defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS)) && defined(UNIASSET_SHARED)
#ifdef UNIASSET_BUILD
#define UNIASSET_API __declspec(dllexport)
#else
#define UNIASSET_API __declspec(dllimport)
#endif
#else
#define UNIASSET_API
#endif

#define CBINDING_TYPED_PTR(TYPE) void*
#define CBINDING_METHOD(RETURN_VALUE, CLASS_NAME, METHOD_NAME, ...) UNIASSET_API RETURN_VALUE Uniasset_##CLASS_NAME##_##METHOD_NAME \
    (__VA_ARGS__)
#define CBINDING_BOOLEAN uint8_t
#define CBINDING_CSTRING const char*
#define CBINDING_ENUM_U8(TYPE) uint8_t
#define CBINDING_CALLBACK void*

#endif //UNIASSET_LIBRARY_HPP
