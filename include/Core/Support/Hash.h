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

extern "C" {
inline void murmur_hash2(u_char *data, size_t len, uint32_t &h) {

    uint32_t k;

    while (len >= 4) {
        k = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= 0x5bd1e995;
        k ^= k >> 24;
        k *= 0x5bd1e995;

        h *= 0x5bd1e995;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len) {
        case 3:
            h ^= data[2] << 16;/* fall through */
        case 2:
            h ^= data[1] << 8;/* fall through */
        case 1:
            h ^= data[0];
            h *= 0x5bd1e995;
    }

    h ^= h >> 13;
    h *= 0x5bd1e995;
    h ^= h >> 15;
}

inline void SimpleHash(uint32_t &Hash, u_char C) {
    Hash = Hash * 31 + C;
}

}