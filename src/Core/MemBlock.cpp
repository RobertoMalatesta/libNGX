#include <cstdlib>
#include <cstring>

#include "Core/Core.h"
#include "Core/MemBlock.h"

namespace ngx::Core {

	MemBlock *MemBlock::New( size_t Size ) {
	
		MemBlock *ret = (MemBlock *) valloc( Size );

        memset(ret, 0, Size);

		if (nullptr == ret) {
			return nullptr;
		}

		ret -> PointerToData = ret + sizeof(MemBlock);
        ret -> PointerToHead = ret + sizeof(MemBlock);
        ret -> FreeSize = Size - sizeof(MemBlock);
        ret -> TotalSize = Size;

		ret -> Magic1 = MemBlockMagic;
		ret -> Magic2 = (unsigned long)ret;

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

    void MemBlock::Free(void *pointer) {
        if (pointer >= PointerToHead &&
            pointer <= ((u_char *)PointerToHead + TotalSize)) {
            UseCount -= 1;
        }
    }

}
