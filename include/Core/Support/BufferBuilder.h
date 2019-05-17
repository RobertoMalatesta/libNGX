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

/**
 *  @name BufferBuilder
 *
 *  @brief Agent to build buffer, helps to collect and reuse buffer memory block
 * */
class BufferBuilder {

protected:

    /** Backend collector to manage memory block */
    BufferMemoryBlockCollector BackendCollector;

public:

    /** Build and configure a Buffer */
    BufferBuilder(size_t BlockSize = BUFFER_MEMORY_BLOCK_SIZE, uint32_t CollectorSize = DEFAULT_COLLECTOR_SIZE);

    ~BufferBuilder();

    /** Set Buffer Block Size, should clear the collector */
    RuntimeError SetBlockSize();

    /** Set Buffer Block Size, should clear the collector */
    RuntimeError SetCollectorSize();

    /** Build and configure a Buffer */
    bool BuildBuffer(Buffer &Buf);
};
