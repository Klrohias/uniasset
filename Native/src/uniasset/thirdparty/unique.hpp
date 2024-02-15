//
// Created by qingy on 2024/2/15.
//

#pragma once
#ifndef UNIASSET_UNIQUE_HPP
#define UNIASSET_UNIQUE_HPP

#include <memory>

namespace uniasset {
    template<typename T>
    using c_deleter = void (*)(T*);

    template<typename T, c_deleter<T> deleter>
    using c_unique_ptr = std::unique_ptr<T, decltype(deleter)>;

    template<typename T, c_deleter<T> deleter>
    c_unique_ptr<T, deleter> make_c_unique(T* ptr) {
        return c_unique_ptr<T, deleter>{ptr, deleter};
    }
}

#endif //UNIASSET_UNIQUE_HPP
