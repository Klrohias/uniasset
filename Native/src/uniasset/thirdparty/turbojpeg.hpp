//
// Created by qingy on 2024/2/15.
//

#pragma once
#ifndef UNIASSET_TURBOJPEG_HPP
#define UNIASSET_TURBOJPEG_HPP

#include <turbojpeg.h>

namespace uniasset {
    inline void tj_deleter(tjhandle obj) {
        tjDestroy(obj);
    }

    inline void tj_deleter(unsigned char* obj) {
        tjFree(obj);
    }
}

#endif //UNIASSET_TURBOJPEG_HPP
