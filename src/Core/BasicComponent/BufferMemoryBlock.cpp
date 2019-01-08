#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

BufferMemoryBlock::BufferMemoryBlock(size_t Size) : BasicMemoryBlock(Size), Recyclable() {

    /**fix start position*/
    Start = (u_char *) this + sizeof(BufferMemoryBlock);
    Reset();
}

BufferMemoryBlock::~BufferMemoryBlock() {
    Pos = nullptr;
}

BufferMemoryBlock* BufferMemoryBlock::AddressToMemoryBlock(void *Address, size_t Size) {
    return (BufferMemoryBlock *)BasicMemoryBlock::AddressToMemoryBlock(Address, Size);
}

void BufferMemoryBlock::Reset() {
    Pos = Start, NextBlock = nullptr;
}
