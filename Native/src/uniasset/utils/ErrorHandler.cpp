//
// Created by Twiiz on 2024/1/14.
//

#include "ErrorHandler.hpp"

#include <stdexcept>

namespace uniasset::utils {
    void ErrorHandler::setError(const std::string& message) {
        message_ = message;
    }

    const std::string& ErrorHandler::getError() const {
        return message_;
    }

    bool ErrorHandler::run(const std::function<void()>& callfunc) {
        try {
            callfunc();
            return true;
        } catch (const std::runtime_error& ex) {
            setError(ex.what());
            return false;
        }
    }

    void ErrorHandler::clear() {
        message_.clear();
    }
} // Uniasset