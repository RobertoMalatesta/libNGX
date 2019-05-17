#include "Core/Core.h"

using namespace ngx::Core::Support;

BasicMemoryBlock::BasicMemoryBlock(size_t Size) : BlockSize(Size) {

    Start = (u_char *) ((size_t) this & ~(BlockSize - 1)) + sizeof(BasicMemoryBlock) + 1;
    End = (u_char *) ((size_t) this & ~(BlockSize - 1)) + BlockSize - 1;

    Magic = (void *) this;
}

BasicMemoryBlock::~BasicMemoryBlock() {
    Magic = nullptr;
}

BasicMemoryBlock *BasicMemoryBlock::AddressToMemoryBlock(void *Address, size_t Size) {

    auto MemBlk = (BasicMemoryBlock *) ((size_t) Address & ~(Size - 1));

    if (MemBlk != nullptr && MemBlk->Magic == (void *) MemBlk && MemBlk->BlockSize == Size) {
        return MemBlk;
    }

    return nullptr;
}
