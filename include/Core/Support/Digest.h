//===- Digest.h - provide digest functions and classes -------------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file contains digest related functions and context classes
//
//===-------------------------------------------------------------------------===//
#include <cstdint>
#include "config_header.h"

#ifndef LIBNGX_CORE_SUPPORT_DISGET
#define LIBNGX_CORE_SUPPORT_DISGET
namespace ngx {
namespace Core {
namespace Support {
namespace Md5 {

using namespace Config;
using namespace std;

inline uint32_t F(uint32_t x, uint32_t y, uint32_t z) { return (z) ^ ((x) & ((y) ^ (z))); }
inline uint32_t G(uint32_t x, uint32_t y, uint32_t z) { return (y) ^ ((z) & ((x) ^ (y))); }
inline uint32_t H(uint32_t x, uint32_t y, uint32_t z) { return (x) ^ (y) ^ (z); }
inline uint32_t I(uint32_t x, uint32_t y, uint32_t z) { return (y) ^ ((x) | ~(z)); }

static uint32_t block[16];

#define STEP(f, a, b, c, d, x, t, s)                                          \
    (a) += f((b), (c), (d)) + (x) + (t);                                      \
    (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s))));                \
    (a) += (b)

inline uint32_t SET(uint32_t n, const u_char *p) {

    if (LITTLE_ENDING && NON_ALIGNED) {
        return (*(uint32_t *) &p[n * 4]);
    } else {
        return block[n] = ((uint32_t) p[n * 4])
                          | ((uint32_t) p[n * 4 + 1] << 8)
                          | ((uint32_t) p[n * 4 + 2] << 16)
                          | ((uint32_t) p[n * 4 + 3] << 24);
    }
}

inline uint32_t GET(uint32_t n, const u_char *p) {
    if (LITTLE_ENDING && NON_ALIGNED) {
        return (*(uint32_t *) &p[n * 4]);
    } else {
        return block[n];
    }
};

} // Md5
} // Support
} // Core
} // ngx
#endif
