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

    void *Magic = nullptr;
    BufferMemoryBlock *MemBlk;

    MemBlk = (BufferMemoryBlock *) ((size_t) Address & ~(Size - 1));

    if (MemBlk != nullptr) {

        Magic = (void *)((size_t) MemBlk->Magic & ~(Size - 1));

        if(Magic == MemBlk && MemBlk->TotalSize == Size) {
            return MemBlk;
        }
    }

    return nullptr;
}

void BufferMemoryBlock::Reset() {
    Pos = Start, NextBlock = nullptr;
}
