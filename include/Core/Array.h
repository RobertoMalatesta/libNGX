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