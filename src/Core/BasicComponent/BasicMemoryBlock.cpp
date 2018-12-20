#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

BasicMemoryBlock::BasicMemoryBlock(size_t Size): TotalSize(Size) {
    Start = (u_char *)this + sizeof(BasicMemoryBlock);
    End = (u_char *)this + Size;
    Magic = (void *) this;
}

BasicMemoryBlock::~BasicMemoryBlock() {
    Start = End = nullptr;
    Magic = nullptr;
}

BasicMemoryBlock *BasicMemoryBlock::AddressToMemoryBlock(void *Address, size_t Size) {

    BasicMemoryBlock *MemBlk;
    MemBlk = (BasicMemoryBlock *) ((size_t) Address & ~(Size - 1));

    if (MemBlk != nullptr && MemBlk->Magic == (void *) MemBlk && MemBlk->TotalSize == Size) {
        return MemBlk;
    }

    return nullptr;
}

bool BasicMemoryBlock::IsInBlock(void *Address) {
    return (Address >= Start && Address < End);
}
