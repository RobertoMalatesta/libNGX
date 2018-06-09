#include "Core/Core.h"

namespace ngx::Core {

    Pool::Pool(size_t BlockSize) {

        if (BlockSize < 1024) {
            BlockSize = 1024;
            //[WARNING] Block Size too small
        }

        this -> BlockSize = BlockSize;
        HeadBlock = MemBlock::New(BlockSize);
        CurrentBlock = HeadBlock;
    }

    void *Pool::Allocate(size_t Size) {

        void *ret = nullptr;

        if (Size >  BlockSize - 4 * sizeof(MemBlock)) {
            ret = malloc(Size);
        }
        else {
            do {
                ret = CurrentBlock -> Allocate(Size);

                if (ret == nullptr) {
                    if (CurrentBlock -> GetNext() == nullptr) {
                        CurrentBlock -> SetNext(MemBlock::New(BlockSize));
                    }
                    CurrentBlock = CurrentBlock->GetNext();
                } else {
                    break;
                }
            } while (true);
        }
        return ret;
    }

    void Pool::Free(void **pointer) {

        MemBlock *TempBlock = HeadBlock;

        if (nullptr != pointer) {

            while (nullptr != TempBlock) {
                if (TempBlock -> IsInBlock(*pointer)) {
                    TempBlock -> Free(pointer);
                    goto Out;
                }
                TempBlock = TempBlock->GetNext();
            }

            free(*pointer);

            Out:
                *pointer = nullptr;
        }
    }

    void Pool::GC() {

        MemBlock *Last = HeadBlock, *Current = nullptr, *Next = nullptr, *TempFreeBlockHead = nullptr, *TempFreeBlockTail = nullptr;

        Current = Last -> GetNext();

        if (HeadBlock -> IsFreeBlock()) {
            HeadBlock -> Reset();
        }

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

        Current = TempFreeBlockHead;

        while (Current != nullptr) {

            Next = Current -> GetNext();
            MemBlock::Destroy(Current);
            Current = Next;
        }

        CurrentBlock = HeadBlock;
    };

    void Pool::Reset() {

        MemBlock *TempMemBlock = HeadBlock;

        while (TempMemBlock != nullptr) {
            TempMemBlock -> Reset();
            TempMemBlock = TempMemBlock -> GetNext();
        }
        CurrentBlock = HeadBlock;
    }

    Pool::~Pool() {
        MemBlock *TempMemBlock = HeadBlock, *Next = nullptr;

        while (TempMemBlock != nullptr) {
            Next = TempMemBlock -> GetNext();
            MemBlock::Destroy(TempMemBlock);
            TempMemBlock = Next;
        }
        HeadBlock = CurrentBlock = nullptr;
    }
}