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
    namespace Utils {
        bool IsWebPFileData(const uint8_t* data, size_t len);

        bool IsJpegFileData(const uint8_t* data, size_t len);

        bool IsMp3FileData(const uint8_t* data, size_t len);

        bool IsFlacFileData(const uint8_t* data, size_t len);

        bool IsWavFileData(const uint8_t* data, size_t len);

        bool IsOggFileData(const uint8_t* data, size_t len);

        void CFileDeleter(FILE* file);

        using CFilePtr = std::unique_ptr<FILE, decltype(Utils::CFileDeleter)*>;

        // from https://stackoverflow.com/questions/13816850/is-it-possible-to-develop-static-for-loop-in-c
        template<int First, int Last>
        struct static_for {
            template<typename Fn>
            void operator()(Fn const& fn) const {
                if (First < Last) {
                    fn(First);
                    static_for<First + 1, Last>()(fn);
                }
            }
        };

        template<int N>
        struct static_for<N, N> {
            template<typename Fn>
            void operator()(Fn const& fn) const {}
        };

    };
} // Uniasset

#endif //UNIASSET_UTILS_HPP
