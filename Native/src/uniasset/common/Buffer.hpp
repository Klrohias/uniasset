//
// Created by qingy on 2024/2/15.
//

#pragma once
#ifndef UNIASSET_BUFFER_HPP
#define UNIASSET_BUFFER_HPP

#include <cstdint>
#include <memory>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace uniasset {
    using BufferDeleter = std::function<void(uint8_t*)>;

    using Buffer = std::unique_ptr<uint8_t[], BufferDeleter>;
}
#endif //UNIASSET_BUFFER_HPP
