//===------------------ Pool.h - Implement memory pool ------------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  define and implement pool
//
//===-------------------------------------------------------------------------===//

class Pool : public Allocator, public CanReset, public CanGC {

private:

    uint32_t AllocateRound = 0;

protected:

    size_t BlockSize;
    MemoryBlockAllocator *HeadBlock, *CurrentBlock;

public:

    Pool(size_t BlockSize = POOL_MEMORY_BLOCK_SIZE);

    Pool(Pool &Copy);

    ~Pool();

    virtual void *Allocate(size_t Size);

    virtual void Free(void *&pointer);

    virtual void GC();

    virtual void Reset();
};