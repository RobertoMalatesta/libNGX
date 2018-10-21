#include "Core/Core.h"

using namespace ngx::Core;

BufferMemoryBlock::BufferMemoryBlock(size_t Size) : MemoryBlock(Size), Recyclable() {
    TotalSize = Size - sizeof(BufferMemoryBlock);
    FreeSize = TotalSize;
    PointerToHead = (u_char *)this + sizeof(BufferMemoryBlock);
    PointerToData = PointerToHead;
    Start = Pos = (u_char *)PointerToHead;
    End = Start + TotalSize;
}

BufferMemoryBlock::~BufferMemoryBlock() {
    Start = Pos = End = nullptr;
}

BufferMemoryBlock* BufferMemoryBlock::Build(size_t Size) {
    void *TempPointer = valloc(Size);

    if (nullptr == TempPointer) {
        return nullptr;
    }

    return new(TempPointer) BufferMemoryBlock(Size);
}

void BufferMemoryBlock::Destroy(BufferMemoryBlock **PointerToBlock) {

    if (nullptr == PointerToBlock || nullptr == *PointerToBlock) {
        return;
    }

    free((void*)*PointerToBlock);
    *PointerToBlock = nullptr;
}

void BufferMemoryBlock::Reset() {
    PointerToHead = (u_char *)this + sizeof(BufferMemoryBlock);
    PointerToData = PointerToHead;
    Start = Pos = (u_char *)PointerToHead;
    End = Start + TotalSize;
    NextBlock = nullptr;
}
