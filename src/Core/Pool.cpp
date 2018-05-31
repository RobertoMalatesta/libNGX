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

    void Pool::GC() {

        MemBlock *Last = HeadBlock, *Current = nullptr, *Next = nullptr, *TempFreeBlockHead = nullptr, *TempFreeBlockTail = nullptr;

        Current = Last -> GetNext();

        while (Current != nullptr) {

            if (Current->IsFreeBlock()) {

                Last->SetNext(Current->GetNext());
                Current->SetNext(nullptr);
                Current -> Reset();

                if (nullptr == TempFreeBlockTail) {
                    TempFreeBlockHead = TempFreeBlockTail = Current;
                } else {
                    TempFreeBlockTail->SetNext(Current);
                    TempFreeBlockTail = TempFreeBlockTail->GetNext();
                }
            }
            else {
                Last = Current;
            }
            Current = Last -> GetNext();
        }

        if (nullptr == TempFreeBlockHead) {
            return;
        }

        Current = Next = TempFreeBlockHead;

        while (Current != nullptr) {

            Next = Current -> GetNext();
            MemBlock::Destroy(Current);
            Current = Next;
        }

//        int reserve = PoolReserveMemBlockCount;
//
//        while (reserve -- >0 && Next != nullptr) {
//            Current = Next;
//            Next = Next ->GetNext();
//        }
//
//        Current -> SetNext(nullptr);
//
//        Current = Next;
//
//        while (Current != nullptr) {
//
//            Next = Current -> GetNext();
//            MemBlock::Destroy(Current);
//            Current = Next;
//        }
//
//        Last -> SetNext(TempFreeBlockHead);

    };
}