//===------- Cursor.h - Cursor to index Buffer memory -----------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===----------------------------------------------------------------------===//
//
//  This file defines Cursor. Cursor helps to index data in Buffer
//
//===----------------------------------------------------------------------===//
#include <cstdlib>
#include <cstdint>

#include "Core/Support/BufferMemoryBlock.h"

#ifndef LIBNGX_CORE_SUPPORT_CURSOR
#define LIBNGX_CORE_SUPPORT_CURSOR

namespace ngx {
namespace Core {
namespace Support {

class Buffer;

/**
 *  @name Cursor
 *
 *  @brief Cursor to indicate a buffer position.
 * */
struct Cursor {
    /** Memory address */
    u_char *Position = nullptr;
    /** Target buffer */
    Buffer *ParentBuffer = nullptr;

    Cursor() = default;
    Cursor(Buffer *ParentBuffer, u_char *Position);

    /** Read cursor data */
    inline u_char operator*() const {
        return (!*this) ? (u_char) '\0' : *Position;
    }
    /** If this cursor available */
    inline bool operator!() const {
        return Position == nullptr;
    }
    /** Assign a cursor to another */
    inline Cursor &operator=(Cursor const &Right) {
        ParentBuffer = Right.ParentBuffer, Position = Right.Position;
        return *this;
    }
    /** Compare if two cursor point to same address */
    inline bool operator==(Cursor const &Right) const {
        return Position == Right.Position;
    }
    /** Get parent buffer of the cursor */
    inline Buffer *GetParentBuffer() {
        return ParentBuffer;
    }
    /** Get parent memory block */
    BufferMemoryBlock *GetParentBlock();
};

} // Support
} // Core
} // ngx

#endif