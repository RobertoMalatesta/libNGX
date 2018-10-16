#include "Http/Http.h"

namespace ngx::Http {

    class ReadBuffer {

        /*
         * size_t BlockSize
         * uint_32 MaxBlocks;
         *
         * Spinlock Lock;
         *
         * BufferMemoryBlock HeadBlock;
         * Cursor ReadCursor, WriteCursor;
         *
         * bool inline ReadByte (Cursor, Offset, u_char &C1)
         * bool inline ReadBytes2 (Cursor, Offset, u_char &C1, u_char &C2)
         * bool inline ReadBytes4 (Cursor, Offset, u_char &C1, u_char &C2, u_char &C3, u_char &C4)
         * bool inline CompareByte (Cursor, Offset, u_char C1)
         * bool inline CompareBytes2 (Cursor, Offset, u_char C1, u_char C2)
         * bool inline CompareBytes4 (Cursor, Offset, u_char C1, u_char C2, u_char C3, u_char C4)
         * Cursor inline GetReadCursor()
         * Cursor inline MoveReadCursor(Offset = 0)
         * Cursor inline MoveCursor(Cursor, Offset)
         * RuntimeError WriteBytes(u_char *Pointer, size_t Length)
         * RuntimeError WriteConnection(Connection *C)
         * Range CreateRange(Cursor1, Cursor2)
         * void DestroyRange(Range)
         * void GC();
         *
         * */
    };
}