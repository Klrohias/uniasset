//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_AUDIOASSET_HPP
#define UNIASSET_AUDIOASSET_HPP

#include <memory>

#include "uniasset/Foundation.hpp"

namespace Uniasset {
    class IAudioDecoder;

    class UNIASSET_API AudioAsset {
    private:
        std::shared_ptr<IAudioDecoder> audioDecoder_{nullptr};

    public:
        explicit AudioAsset();

        ~AudioAsset();
    };

} // Uniasset

#endif //UNIASSET_AUDIOASSET_HPP
