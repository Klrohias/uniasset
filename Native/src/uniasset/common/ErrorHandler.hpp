//
// Created by Twiiz on 2024/1/14.
//

#pragma once
#ifndef UNIASSET_ERRORHANDLER_HPP
#define UNIASSET_ERRORHANDLER_HPP

#include <string>
#include <functional>

namespace Uniasset {

    class ErrorHandler {
        std::string message_{};
    public:
        void SetError(const std::string& message);

        const std::string& GetError() const;

        bool Run(const std::function<void()>& callfunc);
    };

} // Uniasset

#endif //UNIASSET_ERRORHANDLER_HPP
