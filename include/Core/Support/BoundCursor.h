//===- BoundCursor.h - Cursor to index Buffer memory -----------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===----------------------------------------------------------------------===//
//
//  This file defines BoundCursor
//
//===----------------------------------------------------------------------===//
#include "Core/Support/Cursor.h"

#ifndef LIBNGX_CORE_SUPPORT_BOUNDCURSOR
#define LIBNGX_CORE_SUPPORT_BOUNDCURSOR
namespace ngx {
namespace Core {
namespace Support {

class Buffer;

/**
 *  @name BoundCursor
 *
 *  @brief Cursor with right bound to support memory access
 * */
struct BoundCursor : public Cursor {
    u_char *Bound = nullptr;

    BoundCursor() = default;
    BoundCursor(Buffer *ParentBuffer, u_char *Position, u_char *Bound);

    BoundCursor operator+(size_t Size) const;
    /** Increment the cursor pointer */
    inline BoundCursor operator+=(size_t Size) {
        return *this = *this + Size;
    }
    /** Increment the cursor pointer by one */
    inline BoundCursor operator++() {
        return *this = *this + 1;
    }
    /** Increment the cursor pointer by one */
    inline const BoundCursor operator++(int) {
        BoundCursor Ret = *this;
        *this = *this + 1;
        return Ret;
    }
    /** If this cursor available */
    bool operator!();
    /** Read target memory address by index */
    inline u_char operator[](uint16_t Offset) const {
        return *(*this + Offset);
    }
    /** Assign BoundCursor to another */
    inline BoundCursor &operator=(BoundCursor const &Right) &{
        ParentBuffer = Right.ParentBuffer, Position = Right.Position, Bound = Right.Bound;
        return *this;
    }
    /** Set left bound to current bound cursor */
    inline BoundCursor &operator<(Cursor Cursor) &{
        ParentBuffer = Cursor.ParentBuffer, Position = Cursor.Position;
        return *this;
    }
    /** Set right bound to current bound cursor */
    inline BoundCursor &operator>(Cursor Cursor) &{

        ParentBuffer = Cursor.ParentBuffer, this->Bound = Cursor.Position;
        return *this;
    }
    /** Set left bound to current bound cursor */
    inline BoundCursor &operator<(BoundCursor Cursor) &{
        ParentBuffer = Cursor.ParentBuffer;
        Position = Cursor.Position;
        return *this;
    }
    /** Set right bound to current bound cursor */
    inline BoundCursor &operator>(BoundCursor Cursor) &{
        ParentBuffer = Cursor.ParentBuffer;
        this->Bound = Cursor.Position;
        return *this;
    }
    /** Decrement is not allowed */
    BoundCursor operator-(size_t) = delete;
    /** Decrement is not allowed */
    BoundCursor &operator--() = delete;
    /** Decrement is not allowed */
    BoundCursor &operator-=(size_t Size) = delete;
    /** Read data from cursor position */
    size_t ReadBytes(u_char *Pointer, size_t Size);
    /** Compare cursor data with input */
    bool CmpBytes(u_char *Pointer, size_t Size);
    /** Total bytes of cursor range data */
    size_t Size();
};

} // Support
} // Core
} // ngx


#endif
