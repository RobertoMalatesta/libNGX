//===- BufferMemoryBlock.h - Hold a aligned memory for buffer ------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file declare a BufferMemoryBlock to hold page aligned memory block to
//  construct buffers.
//
//===-------------------------------------------------------------------------===//

class BufferMemoryBlock
        : public BasicMemoryBlock,
          public Recyclable,
          public AlignBuild<BufferMemoryBlock, BUFFER_MEMORY_BLOCK_SIZE> {
protected:
    u_char *Pos;
    BufferMemoryBlock *NextBlock = nullptr;

    friend class Buffer;

    friend class BoundCursor;

public:

    BufferMemoryBlock(size_t Size);

    ~BufferMemoryBlock();

    void SetNextBlock(BufferMemoryBlock *Next) { this->NextBlock = Next; }

    BufferMemoryBlock *GetNextBlock() { return NextBlock; }

    virtual void Reset();

    static BufferMemoryBlock *AddressToMemoryBlock(void *Address, size_t Size);

    static inline BufferMemoryBlock *FromRecycleQueue(Queue *Q) {

        if (Q == nullptr) {
            return nullptr;
        }
        return (BufferMemoryBlock *) ((uintptr_t) Q - (uintptr_t) (&((BufferMemoryBlock *) 0)->RecycleItem));
    }
};
