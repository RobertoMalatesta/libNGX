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
    BufferMemoryBlock *NextBlock = nullptr;

    friend class Buffer;

public:
    u_char *Start, *End;

    BufferMemoryBlock(size_t Size);

    ~BufferMemoryBlock();

    void SetNextBlock(BufferMemoryBlock *Next) { this->NextBlock = Next; }

    BufferMemoryBlock *GetNextBlock() { return NextBlock; }

    virtual void Reset();
};
