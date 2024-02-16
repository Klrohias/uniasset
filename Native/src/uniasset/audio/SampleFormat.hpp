//
// Created by Twiiz on 2024/1/20.
//

#pragma once
#ifndef UNIASSET_SAMPLEFORMAT_HPP
#define UNIASSET_SAMPLEFORMAT_HPP

#include <cstdint>

namespace uniasset {
    enum SampleFormat : uint8_t {
        Uint8,
        Int32,
        Float,
        Int16,
    };
}

#endif //UNIASSET_SAMPLEFORMAT_HPP
