//
// Created by qingy on 2024/2/15.
//

#pragma once
#ifndef UNIASSET_TEMPLATES_HPP
#define UNIASSET_TEMPLATES_HPP
#include <cstdlib>

namespace uniasset {
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

    template<typename T>
    inline void default_deleter(const T* buffer) {
        delete buffer;
    }

    template<typename T>
    inline void default_array_deleter(const T* buffer) {
        delete[] buffer;
    }

    template<typename T>
    inline void default_free_deleter(const T* buffer) {
        static_assert(std::is_trivially_destructible<T>::value, "Objects with non-trivial destructors should not be freed using `free`.");
        free(reinterpret_cast<void*>(const_cast<T*>(buffer)));
    }

    template<typename NumType>
    requires std::is_integral_v<NumType>
    inline void* ptr_offset(void* ptr, NumType offset) {
        return (char*) ptr + offset;
    }
}

#endif //UNIASSET_TEMPLATES_HPP
