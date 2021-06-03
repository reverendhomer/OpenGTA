#pragma once

#include <cstddef>
#include <physfs.h>
#include <vector>

namespace OpenGTA {

struct LoadedAnim {
    LoadedAnim(size_t size)
        : frame(size)
    {}
    PHYSFS_uint8 block;
    PHYSFS_uint8 which;
    PHYSFS_uint8 speed;
    PHYSFS_uint8 frameCount;
    std::vector<PHYSFS_uint8> frame;
};

} // namespace OpenGTA
