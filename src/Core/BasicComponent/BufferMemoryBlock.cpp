#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

BufferMemoryBlock::BufferMemoryBlock(size_t Size) : BasicMemoryBlock(Size), Recyclable() {
    Magic = (void *)this;
    Reset();
}

BufferMemoryBlock::~BufferMemoryBlock() {
    Start = Pos = End = nullptr;
}

void BufferMemoryBlock::Reset() {
    Start = Pos = (u_char *)this + sizeof(BufferMemoryBlock);
    End = Start + TotalSize;
    NextBlock = nullptr;
}
