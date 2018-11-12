#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

BufferMemoryBlock::BufferMemoryBlock(size_t Size) : BasicMemoryBlock(Size), Recyclable() {
    TotalSize = Size - sizeof(BufferMemoryBlock);
    FreeSize = TotalSize;
    PointerToHead = (u_char *) this + sizeof(BufferMemoryBlock);
    PointerToData = PointerToHead;
    Start = (u_char *) PointerToHead;
    End = Start + TotalSize;
}

BufferMemoryBlock::~BufferMemoryBlock() {
    Start = End = nullptr;
}

void BufferMemoryBlock::Reset() {
    PointerToHead = (u_char *) this + sizeof(BufferMemoryBlock);
    PointerToData = PointerToHead;
    Start = (u_char *) PointerToHead;
    End = Start + TotalSize;
    NextBlock = nullptr;
}
