//===- BufferBuilder.h - Config Buffer &  create buffer instance ---*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file contains BufferBuilder specification to delegate Buffer construction
//  and configuration
//
//===-------------------------------------------------------------------------===//

class BufferBuilder {
protected:
    u_char Lg2BlockSize;
    BufferMemoryBlockRecycleBin RecycleBin;
public:
    BufferBuilder(u_char Lg2BlockSize = LG2_BUFFER_MEMORY_BLOCK_SIZE, uint64_t RecycleSize = DEFAULT_RECYCLE_SIZE);

    ~BufferBuilder();

    bool BuildBuffer(Buffer &Buf);

    BufferAllocator BuildBufferAllocator();
};
