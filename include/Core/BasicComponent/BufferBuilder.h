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
    size_t BlockSize;
    BufferMemoryBlockRecycleBin RecycleBin;
public:
    BufferBuilder(size_t BlockSize = BUFFER_MEMORY_BLOCK_SIZE, uint64_t RecycleSize = DEFAULT_RECYCLE_SIZE);

    ~BufferBuilder();

    /** Build and configure a Buffer */
    bool BuildBuffer(Buffer &Buf);
};
