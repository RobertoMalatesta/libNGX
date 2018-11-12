#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

MemoryBlockAllocator::MemoryBlockAllocator(size_t Size) : BasicMemoryBlock(Size) {
    PointerToHead = (u_char *) this + sizeof(MemoryBlockAllocator);
    TotalSize = Size - sizeof(MemoryBlockAllocator);
    Reset();
    Magic = (void *) this;
}

MemoryBlockAllocator::~MemoryBlockAllocator() {
    BasicMemoryBlock::~BasicMemoryBlock();
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

void MemoryBlockAllocator::Free(void* &Pointer) {
    if (nullptr != Pointer && IsInBlock(Pointer)) {
        Pointer = nullptr;
        DecRef();
    }
}
