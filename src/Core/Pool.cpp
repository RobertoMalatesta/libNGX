#include <cstdlib>

#include "Core/Core.h"

namespace ngx::Core {

    Pool::Pool(size_t BlockSize) {

        this -> BlockSize = BlockSize;
        HeadBlock = MemBlock::New(BlockSize);
        CurrentBlock = HeadBlock;
    }

    void *Pool::Allocate(size_t Size) {

        void *ret = nullptr;

        if (Size >  BlockSize - 2 * sizeof(MemBlock)) {
            ret = malloc(Size);
        }
        else {
            ret = CurrentBlock -> Allocate(Size);

            if (nullptr == ret) {

                MemBlock *Next = MemBlock::New(BlockSize);

                if (nullptr == Next) {
                    return nullptr;
                }

                CurrentBlock -> SetNext(Next);
                CurrentBlock = Next;

                ret = CurrentBlock->Allocate(Size);
            }
        }

        return ret;
    }

    void Pool::Free(void *pointer) {

        MemBlock *TempBlock = HeadBlock;

        while (nullptr != TempBlock) {
            if (TempBlock -> IsInBlock(pointer)) {
                TempBlock -> Free(pointer);
                return;
            }
            TempBlock = TempBlock->GetNext();
        }
        free(pointer);
    }
}