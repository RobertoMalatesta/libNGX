#include "Core/Core.h"

namespace ngx::Core {

    MemBlock::MemBlock(size_t Size) {
        PointerToHead = (u_char *)this + sizeof(MemBlock);
        TotalSize = Size - sizeof(MemBlock);
        PointerToData = PointerToHead;
        FreeSize = TotalSize;
        Magic = (unsigned long)this;
    }

    MemBlock::~MemBlock() {
        TotalSize = Magic = 0;
        PointerToData = nullptr;
    };

    MemBlock *MemBlock::CreateMemBlock(size_t Size) {

        void *TempPointer = valloc(Size);

        if (nullptr == TempPointer) {
            return nullptr;
        }

        return new(TempPointer) MemBlock(Size);
    }

    void MemBlock::FreeMemBlock(ngx::Core::MemBlock **PointerToBlock) {

        if (nullptr == PointerToBlock || nullptr == *PointerToBlock) {
            return;
        }

        free((void*)*PointerToBlock);
        *PointerToBlock = nullptr;
    }

    MemBlock *MemBlock::AddressToMemBlock(void *Addr, size_t Size) {

        MemBlock * MemBlk = (MemBlock *)( (size_t)Addr & ~(Size - 1));

        if (nullptr != MemBlk && MemBlk -> Magic == (unsigned long)MemBlk) {
            return MemBlk;
        }

        return nullptr;
    }

    void *MemBlock::Allocate(size_t Size) {

        void *ret = nullptr;

        if (FreeSize >= Size) {
            UseCount += 1;
            ret = PointerToData;
            PointerToData = (u_char *)PointerToData + Size;
            FreeSize -= Size;
        }

        return ret;
    }

    bool MemBlock::IsInBlock(void *Address){
        return (Address >= PointerToHead && Address < ((u_char *)PointerToHead + TotalSize));
    }

    void MemBlock::Free(void **pointer) {
        if (nullptr != pointer && IsInBlock(*pointer)) {
            *pointer = nullptr;
            UseCount -= 1;
            if ((UseCount < 0)) {
                //[UNREACHABLE BRANCH] BUG: Over free, will add log here later
            }
        }
    }

    void MemBlock::Reset() {
        PointerToData = PointerToHead;
        FreeSize = TotalSize;
    }
}
