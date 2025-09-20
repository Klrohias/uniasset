//
// Created by qingy on 2024/2/16.
//

#include "Errors.hpp"

#include <miniaudio.h>
#include <stb_image.h>
#include <string>

namespace uniasset {
    namespace internal {
        class error_category : public std::error_category {
            [[nodiscard]] const char* name() const noexcept override {
                return "Uniasset";
            }

            [[nodiscard]] std::string message(int ev) const override {
                return std::to_string(ev);
            }
        };

        class vp8_error_category : public std::error_category {
            [[nodiscard]] const char* name() const noexcept override {
                return "VP8";
            }

            [[nodiscard]] std::string message(int ev) const override {
                return std::to_string(ev);
            }
        };

        class stbi_error_category : public std::error_category {
            [[nodiscard]] const char* name() const noexcept override {
                return "StbImage";
            }

            [[nodiscard]] std::string message(int ev) const override {
                return stbi_failure_reason();
            }
        };

        class turbojpeg_error_category : public std::error_category {
            [[nodiscard]] const char* name() const noexcept override {
                return "TurboJpeg";
            }

            [[nodiscard]] std::string message(int ev) const override {
                return std::to_string(ev);
            }
        };

        class ma_error_category : public std::error_category {
            [[nodiscard]] const char* name() const noexcept override {
                return "Miniaudio";
            }

            [[nodiscard]] std::string message(int ev) const override {
                return ma_result_description(static_cast<ma_result>(ev));
            }
        };
    }

    const std::error_category& uniasset_category() {
        static internal::error_category instance;
        return instance;
    }

    const std::error_category& vp8_category() {
        static internal::vp8_error_category instance;
        return instance;
    }

    const std::error_category& stbi_category() {
        static internal::stbi_error_category instance;
        return instance;
    }

    const std::error_category& turbojpeg_category() {
        static internal::turbojpeg_error_category instance;
        return instance;
    }

    const std::error_category& ma_category()
    {
        static internal::ma_error_category instance;
        return instance;
    }
}
