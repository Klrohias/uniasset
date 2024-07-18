//
// Created by qingy on 2024/2/15.
//

#pragma once
#ifndef UNIASSET_MAGICNUMBERS_HPP
#define UNIASSET_MAGICNUMBERS_HPP

#include <cstring>
#include <cstdint>

namespace uniasset {
    constexpr uint8_t jpegMagicNumber[] = {0xff, 0xd8, 0xff, 0xe0};
    constexpr uint8_t mp3MagicNumberCase1[] = {0x49, 0x44, 0x33};
    constexpr uint8_t mp3MagicNumberCase2[] = {0xff, 0xfb};
    constexpr uint8_t oggMagicNumber[] = {
        0x4F, 0x67, 0x67, 0x53, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00
    };

    inline bool isWebPFileData(const uint8_t* data, size_t len) {
        if (len < 15) {
            return false;
        }

        return data[0] == 'R' && data[1] == 'I' && data[2] == 'F' && data[3] == 'F';
    }

    inline bool isJpegFileData(const uint8_t* data, size_t len) {
        if (len < 4) {
            return false;
        }

        return memcmp(data, jpegMagicNumber, sizeof(jpegMagicNumber)) == 0;
    }

    inline bool isMp3FileData(const uint8_t* data, size_t len) {
        if (len < 3) {
            return false;
        }

        return memcmp(data, mp3MagicNumberCase1, sizeof(mp3MagicNumberCase1)) == 0
               || memcmp(data, mp3MagicNumberCase2, sizeof(mp3MagicNumberCase2)) == 0;
    }

    inline bool isFlacFileData(const uint8_t* data, size_t len) {
        if (len < 4) {
            return false;
        }
        return data[0] == 'f' && data[1] == 'L' && data[2] == 'a' && data[3] == 'C';
    }

    inline bool isWavFileData(const uint8_t* data, size_t len) {
        return isWebPFileData(data, len); // same 'RIFF'
    }

    inline bool isOggFileData(const uint8_t* data, size_t len) {
        if (len < 14) {
            return false;
        }

        return memcmp(data, oggMagicNumber, sizeof(oggMagicNumber)) == 0;
    }
}
#endif //UNIASSET_MAGICNUMBERS_HPP
