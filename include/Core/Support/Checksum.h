//===- Checksum.h - Implement checksum algorithms -------------------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file implements checksum algorithms
//
//===-------------------------------------------------------------------------===//

/* TODO: Align CRC table to cache line size */

extern "C" {

extern uint32_t crc32_table16[];
extern uint32_t crc32_table256[];

static inline uint32_t crc(u_char *data, size_t len) {

    uint32_t sum;

    for (sum = 0; len; len--) {
        sum = sum >> 1 | sum << 31;
        sum += *data++;
    }

    return sum;
}

static inline uint32_t crc32_short(u_char *p, size_t len) {
    u_char c;
    uint32_t crc;

    crc = 0xffffffff;

    while (len--) {
        c = *p++;
        crc = crc32_table16[(crc ^ (c & 0xf)) & 0xf] ^ (crc >> 4);
        crc = crc32_table16[(crc ^ (c >> 4)) & 0xf] ^ (crc >> 4);
    }

    return crc ^ 0xffffffff;
}

static inline uint32_t crc32_long(u_char *p, size_t len) {
    uint32_t crc;

    crc = 0xffffffff;

    while (len--) {
        crc = crc32_table256[(crc ^ *p++) & 0xff] ^ (crc >> 8);
    }

    return crc ^ 0xffffffff;
}

static inline void crc32_update(uint32_t *crc, u_char *p, size_t len) {
    uint32_t c;

    c = *crc;

    while (len--) {
        c = crc32_table256[(c ^ *p++) & 0xff] ^ (c >> 8);
    }

    *crc = c;
}
}
