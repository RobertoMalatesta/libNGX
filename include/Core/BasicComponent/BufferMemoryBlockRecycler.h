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
    BufferMemoryBlockRecycler(size_t BufferMemoryBlockSize, uint64_t RecyclerSize);

    size_t GetBlockSize() { return this->BufferMemoryBlockSize; }

    virtual BufferMemoryBlock *Get();

    virtual void Put(BufferMemoryBlock *Item);
};
