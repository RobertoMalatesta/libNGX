#include <cstdlib>
#include <cstring>

#include "Core/Core.h"

namespace ngx::Core {

	MemBlock *MemBlock::New( size_t Size ) {

        void *TempPointer = valloc(Size);

        if (nullptr == TempPointer) {
            return nullptr;
        }

        memset(TempPointer, 0, Size);

		MemBlock *ret = (MemBlock *) TempPointer;

		ret -> PointerToHead = (u_char *)TempPointer + sizeof(MemBlock);
        ret -> TotalSize = Size - sizeof(MemBlock);

        ret -> PointerToData = ret -> PointerToHead;
        ret -> FreeSize = ret -> TotalSize;

		ret -> Magic1 = MemBlockMagic;
		ret -> Magic2 = (unsigned long)TempPointer;
        ret -> Next = nullptr;

		return ret;
	}
	
	void MemBlock::Destroy( MemBlock *MemBlk ) {
		if ( nullptr != MemBlk &&
             MemBlk -> PointerToData != nullptr &&
             MemBlockMagic == MemBlk->Magic1 &&
             (unsigned long)MemBlk == MemBlk->Magic2) {

            MemBlk -> TotalSize = 0;
            MemBlk -> PointerToData = nullptr;
            MemBlk -> Magic1 = 0;
            MemBlk -> Magic2 = 0;
			free((void *) MemBlk);
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

    void MemBlock::Free(void *pointer) {
        if (IsInBlock(pointer)) {
            UseCount -= 1;
        }
    }

    void MemBlock::Reset() {
        PointerToData = PointerToHead;
        FreeSize = TotalSize;
    }
}
