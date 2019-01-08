//===- Buffer.h - Buffer to provide block-chained buffer --------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===----------------------------------------------------------------------===//
//
//  This file provide Buffer facility to enable block based Buffer
//
//===----------------------------------------------------------------------===//

class Cursor;

class BoundCursor;

/**
 *  @name Buffer
 *
 *  @brief Block based buffer used to store and write connection data
 * */
class Buffer : public CanReset, public CanGC {
protected:
    size_t BlockSize;
    BoundCursor Cursor;
    BufferMemoryBlock *HeadBlock = nullptr;
    BufferMemoryBlockRecycleBin *RecycleBin = nullptr;

    friend class BufferBuilder;

public:
    Buffer() = default;

    ~Buffer();

    /** Set the Buffer BoundCursor, used to update left bound */
    Buffer &operator<<(BoundCursor &BC) &;

    /** Get the Buffer BoundCursor, used to iterate the buffer */
    Buffer &operator>>(BoundCursor &BC) &;

    /** Retrieval the BufferMemoryBlock by the memory address */
    inline BufferMemoryBlock *AddressToMemoryBlock(void *Cursor) const {
        return BufferMemoryBlock::AddressToMemoryBlock(Cursor, BlockSize);
    }

    /** Read Connection data to Buffer and move right bound */
    RuntimeError ReadConnection(Connection *C);

    /** Read raw bytes to Buffer and move right bound */
    RuntimeError ReadBytes(u_char *PointerToData, size_t DataLength);

    /** Write Buffer data to connection and move left bound, return EAGAIN when nonblocking write failed */
    RuntimeError WriteConnection(Connection *C);

    /** Write Buffer data to target buffer */
    RuntimeError WriteBytes(u_char *PointerToData, size_t DataLength);

    /** If the Buffer is empty */
    inline bool IsEmpty() const {
        return !(Cursor + 1);
    }

    /** Release all memory buffer used */
    virtual void Reset();

    /** Release all blocks before read cursor */
    virtual void GC();
};


