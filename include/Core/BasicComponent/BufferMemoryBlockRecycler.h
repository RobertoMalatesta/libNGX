//===- BufferMemoryRecycler.h - Recycler for BufferMemoryBlock -----*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file declare a simple recycler of BufferMemoryBlock, which aims to
//  reduce the count of malloc(2) procedure
//
//===-------------------------------------------------------------------------===//

class BufferMemoryBlockRecycler : public Recycler {
protected:
    SpinLock Lock;
    size_t BufferMemoryBlockSize;
public:
    BufferMemoryBlockRecycler(size_t BufferMemoryBlockSize = BUFFER_DEFAULT_BLOCK_SIZE, uint64_t RecyclerSize = DEFAULT_RECYCLE_SIZE);

    size_t GetBlockSize() { return this->BufferMemoryBlockSize; }

    virtual BufferMemoryBlock *Get();

    virtual void Put(BufferMemoryBlock *Item);
};
