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

namespace ngx{
    namespace Core {
        class BufferBuilder {
        protected:
            size_t BlockSize;
            BufferMemoryBlockRecycler Recycler;
        public:
            BufferBuilder(size_t BlockSize, uint64_t RecycleSize);

            bool BuildBuffer(Buffer &Buf);

            BufferAllocator BuildBufferAllocator();
        };
    }
}