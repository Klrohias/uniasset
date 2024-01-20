//
// Created by Twiiz on 2024/1/14.
//

#include <stdexcept>
#include "Utils.hpp"

namespace Uniasset {

    namespace Utils {

        const uint8_t jpegMagicNumber[] = {0xff, 0xd8, 0xff, 0xe0};
        const uint8_t mp3MagicNumber[] = {0x49, 0x44, 0x33};
        const uint8_t oggMagicNumber[] = {0x4F, 0x67, 0x67, 0x53, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00};

        bool IsWebPFileData(const uint8_t* data, size_t len) {
            if (len < 15) {
                return false;
            }

            return data[0] == 'R' && data[1] == 'I' && data[2] == 'F' && data[3] == 'F';
        }

        bool IsJpegFileData(const uint8_t* data, size_t len) {
            if (len < 4) {
                return false;
            }

            return memcmp(data, jpegMagicNumber, sizeof(jpegMagicNumber)) == 0;
        }

        void CFileDeleter(FILE* file) {
            fclose(file);
        }

        bool IsMp3FileData(const uint8_t* data, size_t len) {
            if (len < 3) {
                return false;
            }

            return memcmp(data, mp3MagicNumber, sizeof(mp3MagicNumber)) == 0;
        }

        bool IsFlacFileData(const uint8_t* data, size_t len) {
            if (len < 4) {
                return false;
            }
            return data[0] == 'f' && data[1] == 'L' && data[2] == 'a' && data[3] == 'C';
        }

        bool IsWavFileData(const uint8_t* data, size_t len) {
            return IsWebPFileData(data, len); // same 'RIFF'
        }

        bool IsOggFileData(const uint8_t* data, size_t len) {
            if (len < 14) {
                return false;
            }

            return memcmp(data, oggMagicNumber, sizeof(oggMagicNumber)) == 0;
        }

    };
} // Uniasset