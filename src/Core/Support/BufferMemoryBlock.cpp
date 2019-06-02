#include "Core/Support/BufferMemoryBlock.h"

using namespace ngx::Core::Support;

BufferMemoryBlock::BufferMemoryBlock(size_t Size) : BasicMemoryBlock(Size), Reusable() {
    /** reset Next position */
    Start = (u_char *) ((size_t) this & ~(BlockSize - 1)) + sizeof(BufferMemoryBlock) + 1;
    End = (u_char *) ((size_t) this & ~(BlockSize - 1)) + BlockSize - 1;
    Reset();
}

BufferMemoryBlock *BufferMemoryBlock::AddressToMemoryBlock(void *Address, size_t Size) {

    void *Magic = nullptr;
    BufferMemoryBlock *MemBlk;

    MemBlk = (BufferMemoryBlock *) ((uintptr_t) Address & ~(Size - 1));

    if (MemBlk != nullptr) {

        Magic = (void *) ((size_t) MemBlk->Magic & ~(Size - 1));

        if (Magic == MemBlk && MemBlk->BlockSize == Size) {
            return MemBlk;
        }
    }

    return nullptr;
}

void BufferMemoryBlock::Reset() {
    NextBlock = nullptr;
}
