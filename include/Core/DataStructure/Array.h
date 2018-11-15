
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

class Array {
private:
    uint NAlloc = 0;
    size_t Size = 0;
    uint32_t ElementCount = 0;
    u_char *PointerToData = nullptr;
    MemAllocator *Allocator = nullptr;

public:
    Array(MemAllocator *Allocator, size_t Size, uint32_t Count = ARRAY_DEFAULT_ELEMENT_COUNT);

    ~Array();

    void *operator[](uint32_t Index) {

        if (PointerToData == nullptr || Index >= ElementCount) {
            return nullptr;
        }

        return PointerToData + (Index * Size);

    }

    void *Push();

    void *PushN(uint N);
};