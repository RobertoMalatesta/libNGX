//===- BufferMemoryRecycleBin.h - RecycleBin for BufferMemoryBlock -----*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file declare a simple RecycleBin of BufferMemoryBlock, which aims to
//  reduce the count of malloc(2) procedure
//
//===-------------------------------------------------------------------------===//

class BufferMemoryBlockRecycleBin : public RecycleBin {
protected:
    SpinLock Lock;
    size_t BufferMemoryBlockSize;
public:
    BufferMemoryBlockRecycleBin(size_t BufferMemoryBlockSize = BUFFER_MEMORY_BLOCK_SIZE,
                                uint64_t RecycleBinSize = DEFAULT_RECYCLE_SIZE);

    ~BufferMemoryBlockRecycleBin();

    size_t GetBlockSize() { return this->BufferMemoryBlockSize; }

    virtual BufferMemoryBlock *Get();

    virtual void Put(BufferMemoryBlock *Item);
};
