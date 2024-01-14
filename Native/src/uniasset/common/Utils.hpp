//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_UTILS_HPP
#define UNIASSET_UTILS_HPP

#include <iostream>
#include <cstdint>
#include <cstdio>

namespace Uniasset {
    class Utils {
    public:
        static bool IsWebPFileData(const uint8_t* data, size_t len);

        static bool IsJpegFileData(const uint8_t* data, size_t len);

        static void CFileDeleter(FILE* file);
    };

    using CFilePtr = std::unique_ptr<FILE, decltype(Utils::CFileDeleter)*>;
} // Uniasset

#endif //UNIASSET_UTILS_HPP
