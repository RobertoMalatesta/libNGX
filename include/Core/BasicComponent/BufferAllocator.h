//===- BufferAllocator.h - Buffer to provide buffer memory alloc ---*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file provide BufferAllocator to enable create small memory pieces to
//  hold data temporarily
//
//===-------------------------------------------------------------------------===//

class BufferAllocator : public CanReset {
    SpinLock Lock;
    size_t BlockSize = 0;
    BufferMemoryBlockRecycleBin *RecycleBin = nullptr;
    Cursor ReadCursor, WriteCursor;
    BufferMemoryBlock *HeadBlock = nullptr, *CurrentBlock = nullptr;

    friend class BufferBuilder;

    BufferAllocator(BufferMemoryBlockRecycleBin *RecycleBin, size_t BlockSize = BUFFER_DEFAULT_BLOCK_SIZE);

    ~BufferAllocator();

    Range AllocateBufferRange(size_t Size);

    void GC();

    void Reset();
};
