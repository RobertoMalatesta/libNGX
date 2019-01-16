#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

MemoryBlockAllocator::MemoryBlockAllocator(size_t Size) : BasicMemoryBlock(Size) {
    Start = (u_char *)((size_t)this & ~(BlockSize - 1)) + sizeof(MemoryBlockAllocator) + 1;
    End  = (u_char *)((size_t)this & ~(BlockSize - 1)) + BlockSize - 1;
    Reset();
}

void *MemoryBlockAllocator::Allocate(size_t Size) {

    void *ret = nullptr;

    if (Pos + Size <= End) {
        IncRef();
        ret = Pos, Pos += Size;
    }

    return ret;
}

void MemoryBlockAllocator::Free(void *&Pointer) {
    if (nullptr != Pointer && IsInBlock(Pointer)) {
        Pointer = nullptr;
        DecRef();
    }
}

void MemoryBlockAllocator::Reset() {
    Pos = Start, Next = nullptr;
    ClearRef();
}