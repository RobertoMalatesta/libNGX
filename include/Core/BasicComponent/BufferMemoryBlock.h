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
          public Reusable,
          public AlignBuild<BufferMemoryBlock, BUFFER_MEMORY_BLOCK_SIZE> {
protected:
    BufferMemoryBlock *NextBlock = nullptr;

    friend class Buffer;

    friend class BoundCursor;

public:

    BufferMemoryBlock(size_t Size);

    ~BufferMemoryBlock() = default;

    /** Get start address from address of start */
    inline u_char *Start() const {
        return (u_char *)((uintptr_t)this & ~(BlockSize - 1)) + sizeof(BufferMemoryBlock);
    }

    /** Get start address from address of end */
    inline u_char *End() const {
        return (u_char *)((uintptr_t)this | (BlockSize - 1));
    }

    virtual void Reset();

    void SetNextBlock(BufferMemoryBlock *Next) { this->NextBlock = Next; }

    BufferMemoryBlock *GetNextBlock() { return NextBlock; }

    static BufferMemoryBlock *AddressToMemoryBlock(void *Address, size_t Size);

    static inline BufferMemoryBlock *FromCollectorQueue(Queue *Q) {

        if (Q == nullptr) {
            return nullptr;
        }
        return (BufferMemoryBlock *) ((uintptr_t) Q - (uintptr_t) (&((BufferMemoryBlock *) 0)->ReuseItem));
    }
};
