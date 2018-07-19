#include "Core/Core.h"

namespace ngx::Core {

    Pool::Pool(size_t BlockSize) {

        if (BlockSize < 1024) {
            BlockSize = 1024;
            //[WARNING] Block Size too small
        }

        this -> BlockSize = BlockSize;
        HeadBlock = MemBlock::CreateMemBlock(BlockSize);
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
                        CurrentBlock -> SetNext( MemBlock::CreateMemBlock(BlockSize));
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

        bool FoundInBlock=false;

        MemBlock *TempBlock = HeadBlock;

        if (nullptr != pointer && nullptr != *pointer) {

            while (nullptr != TempBlock) {
                if (TempBlock -> IsInBlock(*pointer)) {
                    TempBlock -> Free(pointer);
                    FoundInBlock= true;
                    break;
                }
                TempBlock = TempBlock->GetNext();
            }
            if (!FoundInBlock) {
                free(*pointer);
            }
        }
    }

    void Pool::GC() {

        int Residual = DefaultPoolResidual;

        MemBlock *Last = HeadBlock, *Current = nullptr, *Next = nullptr, *TempFreeBlockHead = nullptr, *TempFreeBlockTail = nullptr;

        Current = Last -> GetNext();

        if (HeadBlock -> IsFreeBlock()) {
            HeadBlock -> Reset();
        }

        while (Current != nullptr) {

            if (Current->IsFreeBlock()) {

                Last->SetNext(Current->GetNext());
                Current->SetNext(nullptr);

                if (Residual > 0) {
                    Residual -= 1;
                    Current -> Reset();
                    if (nullptr == TempFreeBlockTail) {
                        TempFreeBlockHead = TempFreeBlockTail = Current;
                    } else {
                        TempFreeBlockTail->SetNext(Current);
                        TempFreeBlockTail = TempFreeBlockTail->GetNext();
                    }
                }
                else {
                    delete Current;
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

        Last -> SetNext(TempFreeBlockHead);
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
            MemBlock::FreeMemBlock(&TempMemBlock);
            TempMemBlock = Next;
        }
        HeadBlock = CurrentBlock = nullptr;
    }
}