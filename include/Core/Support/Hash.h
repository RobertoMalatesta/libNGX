//===------------------ Hash.h - Implement hash functions ---------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  define and implement hash function
//
//===-------------------------------------------------------------------------===//
#include <cstdint>
#include <cstdlib>
#include "Core/Support/MemoryBuffer.h"
#ifndef LIBNGX_CORE_SUPPORT_HASH
#define LIBNGX_CORE_SUPPORT_HASH

namespace ngx {
namespace Core {
namespace Support {

using namespace std;

extern "C" {
const u_char lower_case[] =
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0-\0\0" "0123456789\0\0\0\0\0\0"
        "\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
        "\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

inline void SimpleHash(uint32_t &Hash, u_char C) {
    Hash = Hash * 31 + C;
}
inline uint32_t murmurhash2(StringRef Str, bool lower=false) {
#define or_lower(x) ((lower&&lower_case[x]) ? lower_case[x] : x)
    uint32_t  h, k;
    auto it=Str.begin();
    h = 0^Str.size();
    for(; it<Str.end(); it+=4) {
        k  = or_lower(it[0]);
        k |= or_lower(it[1]) << 8;
        k |= or_lower(it[2]) << 16;
        k |= or_lower(it[3]) << 24;
        k *= 0x5bd1e995;
        k ^= k >> 24;
        k *= 0x5bd1e995;
        h *= 0x5bd1e995;
        h ^= k;
    }
    switch (Str.end()-it) {
        case 3:
            h ^= or_lower(it[2]) << 16;
        case 2:
            h ^= or_lower(it[1]) << 8;
        case 1:
            h ^= or_lower(it[0]);
            h *= 0x5bd1e995;
    }
    h ^= h >> 13;
    h *= 0x5bd1e995;
    h ^= h >> 15;
    return h;
#undef or_lower
}

}
} // Support
} // Core
} // ngx
#endif