#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

BasicMemoryBlock::BasicMemoryBlock(size_t Size) : Reference() {
    PointerToHead = (u_char *) this + sizeof(BasicMemoryBlock);
    TotalSize = Size - sizeof(BasicMemoryBlock);
    PointerToData = PointerToHead;
    FreeSize = TotalSize;
    Magic = (void *) this;
}

BasicMemoryBlock::~BasicMemoryBlock() {
    Magic = nullptr;
    PointerToData = PointerToHead = nullptr;
}

BasicMemoryBlock *BasicMemoryBlock::AddressToMemoryBlock(void *Address, size_t Size) {

    BasicMemoryBlock *MemBlk;
    MemBlk = (BasicMemoryBlock *) ((size_t) Address & ~(Size - 1));

    if (nullptr != MemBlk && MemBlk->Magic == (void *) MemBlk) {
        return MemBlk;
    }

    return nullptr;
}

bool BasicMemoryBlock::IsInBlock(void *Address) {
    return (Address >= PointerToHead && Address < ((u_char *) PointerToData));
}
