//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_ERRORHANDLER_HPP
#define UNIASSET_ERRORHANDLER_HPP

#include <string>
#include <functional>

namespace uniasset::utils {

    class ErrorHandler {
        std::string message_{};
    public:
        void setError(const std::string& message);

        [[nodiscard]] const std::string& getError() const;

        bool run(const std::function<void()>& callfunc);

        void clear();
    };

} // Uniasset

#define ERROR_HANDLER_ERRNO(HANDLER, DESC) { \
    (HANDLER).setError(std::string(DESC " (errno: ") + std::to_string(errno) \
        + " " + strerror(errno) + ")"); \
}; 0

#endif //UNIASSET_ERRORHANDLER_HPP
