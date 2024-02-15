//
// Created by qingy on 2024/2/15.
//

#pragma once
#ifndef UNIASSET_C_FILE_HPP
#define UNIASSET_C_FILE_HPP

#include <cstdio>
#include "uniasset/thirdparty/unique.hpp"

namespace uniasset {
    inline void FILE_deleter(FILE* file) {
        fclose(file);
    }
}

#endif //UNIASSET_C_FILE_HPP
