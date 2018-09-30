#include "Core/Core.h"

namespace ngx::Core {

    MemoryBlock::MemoryBlock(size_t Size) {
        PointerToHead = (u_char *)this + sizeof(MemoryBlock);
        TotalSize = Size - sizeof(MemoryBlock);
        PointerToData = PointerToHead;
        FreeSize = TotalSize;
        Magic = (void *)this;
    }

    MemoryBlock::~MemoryBlock() {
        Magic = nullptr;
        PointerToData = PointerToHead = nullptr;
    }

    MemoryBlock* MemoryBlock::Build(size_t Size) {

        void *TempPointer = valloc(Size);

        if (nullptr == TempPointer) {
            return nullptr;
        }

        return new(TempPointer) MemoryBlock(Size);
    }

    void MemoryBlock::Destroy(MemoryBlock **PointerToBlock) {
        if (nullptr == PointerToBlock || nullptr == *PointerToBlock) {
            return;
        }

        free((void*)*PointerToBlock);
        *PointerToBlock = nullptr;
    }

    MemoryBlock* MemoryBlock::AddressToMemoryBlock(void *Address, size_t Size) {

        MemoryBlock * MemBlk;
        MemBlk = (MemoryBlock *)( (size_t)Address & ~(Size - 1));

        if (nullptr != MemBlk && MemBlk -> Magic ==(void *) MemBlk) {
            return MemBlk;
        }

        return nullptr;
    }

    bool MemoryBlock::IsInBlock(void *Address) {
        return (Address >= PointerToHead && Address < ((u_char *)PointerToData));
    }
}
