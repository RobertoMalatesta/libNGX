#include "Core/Core.h"

using namespace ngx::Core;

MemoryBlockAllocator::MemoryBlockAllocator(size_t Size) : MemoryBlock(Size) {
    PointerToHead = (u_char *) this + sizeof(MemoryBlockAllocator);
    TotalSize = Size - sizeof(MemoryBlockAllocator);
    Reset();
    Magic = (void *) this;
}

MemoryBlockAllocator::~MemoryBlockAllocator() {
    MemoryBlock::~MemoryBlock();
}

MemoryBlockAllocator *MemoryBlockAllocator::Build(size_t Size) {
    void *TempPointer = valloc(Size);

    if (nullptr == TempPointer) {
        return nullptr;
    }

    return new(TempPointer) MemoryBlockAllocator(Size);
}

void MemoryBlockAllocator::Destroy(MemoryBlockAllocator **PointerToAllocator) {
    if (nullptr == PointerToAllocator || nullptr == *PointerToAllocator) {
        return;
    }

    free((void *) *PointerToAllocator);
    *PointerToAllocator = nullptr;
}

void *MemoryBlockAllocator::Allocate(size_t Size) {
    void *ret = nullptr;

    if (FreeSize >= Size) {
        IncRef();
        ret = PointerToData;
        PointerToData = (u_char *) PointerToData + Size;
        FreeSize -= Size;
    }

    return ret;
}

void MemoryBlockAllocator::Free(void **Pointer) {
    if (nullptr != Pointer && IsInBlock(*Pointer)) {
        *Pointer = nullptr;
        DecRef();
    }
}
