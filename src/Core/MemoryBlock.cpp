#include "Core/Core.h"

using namespace ngx::Core;

MemoryBlock::MemoryBlock(size_t Size) {
    PointerToHead = (u_char *) this + sizeof(MemoryBlock);
    TotalSize = Size - sizeof(MemoryBlock);
    PointerToData = PointerToHead;
    FreeSize = TotalSize;
    Magic = (void *) this;
}

MemoryBlock::~MemoryBlock() {
    Magic = nullptr;
    PointerToData = PointerToHead = nullptr;
}

MemoryBlock *MemoryBlock::AddressToMemoryBlock(void *Address, size_t Size) {

    MemoryBlock *MemBlk;
    MemBlk = (MemoryBlock *) ((size_t) Address & ~(Size - 1));

    if (nullptr != MemBlk && MemBlk->Magic == (void *) MemBlk) {
        return MemBlk;
    }

    return nullptr;
}

bool MemoryBlock::IsInBlock(void *Address) {
    return (Address >= PointerToHead && Address < ((u_char *) PointerToData));
}
