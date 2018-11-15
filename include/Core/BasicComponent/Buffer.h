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

class Range;

class Buffer : public CanReset {
protected:
    size_t BlockSize;
    BoundCursor Cursor;
    BufferMemoryBlock *HeadBlock = nullptr;
    BufferMemoryBlockRecycleBin *RecycleBin = nullptr;

    friend class BufferBuilder;

public:
    Buffer() = default;

    ~Buffer();

    Buffer &operator<<(BoundCursor &BC) &;

    Buffer &operator>>(BoundCursor &BC) &;

    inline BufferMemoryBlock *AddressToMemoryBlock(void *Cursor) const {
        return (BufferMemoryBlock *) BufferMemoryBlock::AddressToMemoryBlock(Cursor, BlockSize);
    }

    RuntimeError WriteConnectionToBuffer(Connection *C);

    RuntimeError WriteDataToBuffer(u_char *PointerToData, size_t DataLength);

    inline bool HasBytes(uint32_t Count = 1) {
        return (Cursor + 1).Position == nullptr;
    }

    virtual void Reset();

    virtual void GC();
};


