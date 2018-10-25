//===- Array.h - Array to hold contiguous element   -------------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===----------------------------------------------------------------------===//
//
//  This file provide Array facility to enable alloc contiguous elements
//
//===----------------------------------------------------------------------===//

namespace ngx {
    namespace Core {

        class ArrayMemoryBlock : public MemoryBlock, public Recyclable {
        protected:
            u_char *Start, *End;
            size_t ItemSize, MemoryBlockSize;
            uint32_t MaxItems, AllocatedItems;
        public:
            ArrayMemoryBlock(size_t MemoryBlockSize, size_t ItemSize);

            ~ArrayMemoryBlock();

            void Reset();

            void *Push();

            void *PushN(uint32_t N);

            static ArrayMemoryBlock *Build(size_t MemoryBlockSize, size_t ItemSize);

            static void Destroy(ArrayMemoryBlock **PointerToBlock);
        };

        class Array {
        private:
            uint NAlloc = 0;
            size_t Size = 0;
            Pool *Allocator;
            uint ElementCount = 0;
            void *PointerToData;

        public:
            Array(Pool *Allocator, size_t Size, uint Count);

            ~Array();

            void *Push();

            void *PushN(uint N);
        };


    }
}