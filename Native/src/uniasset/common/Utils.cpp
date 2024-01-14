//
// Created by Twiiz on 2024/1/14.
//

#include <stdexcept>
#include "Utils.hpp"

namespace Uniasset {

    const uint8_t jpegMagicNumber[] = {0xff, 0xd8, 0xff, 0xe0};

    bool Utils::IsWebPFileData(const uint8_t* data, size_t len) {
        if (len < 15) {
            return false;
        }

        return data[0] == 'R' && data[1] == 'I' && data[2] == 'F' && data[3] == 'F';
    }

    bool Utils::IsJpegFileData(const uint8_t* data, size_t len) {
        if (len < 4) {
            return false;
        }
        return memcmp(data, jpegMagicNumber, 4) == 0;
    }

    void Utils::CFileDeleter(FILE* file) {
        fclose(file);
    }
} // Uniasset