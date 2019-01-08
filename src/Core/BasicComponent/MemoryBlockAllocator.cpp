#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

MemoryBlockAllocator::MemoryBlockAllocator(size_t Size) : BasicMemoryBlock(Size) {

    // fix start position
    Start = (u_char *) this + sizeof(MemoryBlockAllocator);
    Reset();
}

void *MemoryBlockAllocator::Allocate(size_t Size) {

    void *ret = nullptr;

    if (Pos + Size <= End) {
        IncRef();

        ret = Pos;
        Pos += Size;
        FreeSize -= Size;
    }

    return ret;
}

void MemoryBlockAllocator::Free(void *&Pointer) {

    if (nullptr != Pointer && IsInBlock(Pointer)) {
        Pointer = nullptr;
        DecRef();
    }
}
