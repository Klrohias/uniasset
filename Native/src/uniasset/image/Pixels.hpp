//
// Created by qingy on 2024/3/10.
//

#pragma once
#ifndef UNIASSET_PIXELS_HPP
#define UNIASSET_PIXELS_HPP

#include <cstdint>

namespace uniasset {
    union Pixel24 {
        uint8_t array[3];
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
    };

    union Pixel32 {
        uint8_t array[4];
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        };
    };

    inline Pixel32 toPixel32(Pixel24& val) {
        return {val.r, val.g, val.b, 255};
    }

    inline Pixel24 toPixel24(Pixel32& val) {
        return {val.r, val.g, val.b};
    }
}

#endif //UNIASSET_PIXELS_HPP
