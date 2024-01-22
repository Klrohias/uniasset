//
// Created by Twiiz on 2024/1/14.
//

#include "ErrorHandler.hpp"

#include <stdexcept>

namespace Uniasset {
    void ErrorHandler::SetError(const std::string& message) {
        message_ = message;
    }

    const std::string& ErrorHandler::GetError() const {
        return message_;
    }

    bool ErrorHandler::Run(const std::function<void()>& callfunc) {
        try {
            callfunc();
            return true;
        } catch (const std::runtime_error& ex) {
            SetError(ex.what());
            return false;
        }
    }

    void ErrorHandler::Clear() {
        message_.clear();
    }
} // Uniasset