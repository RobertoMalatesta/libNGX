#include "Core/Core.h"

namespace ngx::Core {

    const MemBlock constBlock;

    MemBlock *MemBlock::New( size_t Size ) {

        void *TempPointer = valloc(Size);

        if (nullptr == TempPointer) {
            return nullptr;
        }

        memcpy(TempPointer, (void *)&constBlock, sizeof(MemBlock));
        MemBlock *ret = (MemBlock *) TempPointer;

        ret -> PointerToHead = (u_char *)TempPointer + sizeof(MemBlock);
        ret -> TotalSize = Size - sizeof(MemBlock);
        ret -> PointerToData = ret -> PointerToHead;
        ret -> FreeSize = ret -> TotalSize;
        ret -> Magic2 = (unsigned long)TempPointer;

        return ret;
    }

    MemBlock::~MemBlock() {
        TotalSize = Magic1 = Magic2 = 0;
        PointerToData = nullptr;
    };

    void MemBlock::Destroy( MemBlock *MemBlk ) {
        if ( nullptr != MemBlk && MemBlockMagic == MemBlk->Magic1 &&
                (unsigned long)MemBlk == MemBlk->Magic2 &&
                MemBlk -> PointerToData != nullptr) {
                delete MemBlk;
        }
    }

    MemBlock *MemBlock::AddressToMemBlock(void *Addr, size_t Size) {

        MemBlock * MemBlk = (MemBlock *)( (size_t)Addr & ~(Size - 1));

        if (nullptr != MemBlk
            && MemBlk -> Magic1 == MemBlockMagic
            && MemBlk -> Magic2 == (unsigned long)MemBlk) {

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
