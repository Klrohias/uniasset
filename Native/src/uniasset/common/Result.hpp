//
// Created by NightOzZero on 2024/5/3.
//

#pragma once

#ifndef UNIASSET_RESULT_HPP
#define UNIASSET_RESULT_HPP

#include <system_error>
#include <optional>

namespace uniasset {

    template<typename TRaw, typename ERaw = std::error_code>
    struct Result {
    private:
        using T =  std::remove_reference_t<TRaw>;
        using E =  std::remove_reference_t<ERaw>;

    private:
        enum State {
            Object,
            Error,
            Null
        };
        State state_{};
        union {
            T object_;
            E error_;
        };

    public:
        Result(const T& object) : state_{Object}, object_{object} {}

        Result(T&& object) : state_{Object}, object_{std::move(object)} {}

        Result(const E& error) : state_{Error}, error_{error} {}

        Result(E&& error) : state_{Error}, error_{std::move(error)} {}

        Result(Result&& other) noexcept {
            state_ = other.state_;

            if (state_ == Object) {
                object_ = std::move(other.object_);
            } else if (state_ == Error) {
                error_ = std::move(other.error_);
            }

            other.state_ = Null;
        }

        ~Result() {
            if (state_ == Object) {
                object_.~T();
            } else if (state_ == Error) {
                error_.~E();
            }
        }

        std::optional<E*> error() {
            if (state_ != Error) {
                return {};
            }

            return {&error_};
        }

        std::optional<T*> data() {
            if (state_ != Object) {
                return {};
            }

            return {&object_};
        }
    };
}

#endif //UNIASSET_RESULT_HPP
