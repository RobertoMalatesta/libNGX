#include "Core/Core.h"

namespace ngx::Core {

    Pool::Pool(size_t BlockSize) {

        if (BlockSize < 1024) {
            BlockSize = 1024;
            //[WARNING] Block Size too small
        }

        this -> BlockSize = BlockSize;
        HeadBlock = MemoryBlockAllocator::Build(BlockSize);
        CurrentBlock = HeadBlock;
    }

    void *Pool::Allocate(size_t Size) {

        void *ret = nullptr;

        if (Size == 0) {
            return ret;
        }
        else if (Size >  BlockSize - 4 * sizeof(MemoryBlockAllocator)) {
            ret = malloc(Size);
        }
        else {
            do {
                ret = CurrentBlock -> Allocate(Size);

                if (ret == nullptr) {
                    if (CurrentBlock -> GetNextBlock() == nullptr) {
                        CurrentBlock -> SetNextBlock(MemoryBlockAllocator::Build(BlockSize));
                    }
                    CurrentBlock = CurrentBlock->GetNextBlock();
                } else {
                    break;
                }
            } while (true);
        }
        return ret;
    }

    void Pool::Free(void **pointer) {

        bool FoundInBlock=false;

        MemoryBlockAllocator *TempBlock = HeadBlock;

        if (nullptr != pointer && nullptr != *pointer) {

            while (nullptr != TempBlock) {
                if (TempBlock -> IsInBlock(*pointer)) {
                    TempBlock -> Free(pointer);
                    FoundInBlock= true;
                    break;
                }
                TempBlock = TempBlock->GetNextBlock();
            }
            if (!FoundInBlock) {
                free(*pointer);
            }
        }
    }

    void Pool::GC() {

        int Residual = DefaultPoolResidual;

        MemoryBlockAllocator *Last = HeadBlock, *Current = nullptr /*, *Next = nullptr */, *TempFreeBlockHead = nullptr, *TempFreeBlockTail = nullptr;

        Current = Last -> GetNextBlock();

        if (HeadBlock -> IsFreeBlock()) {
            HeadBlock -> Reset();
        }

        while (Current != nullptr) {

            if (Current->IsFreeBlock()) {

                Last->SetNextBlock(Current->GetNextBlock());
                Current->SetNextBlock(nullptr);

                if (Residual > 0) {
                    Residual -= 1;
                    Current -> Reset();
                    if (nullptr == TempFreeBlockTail) {
                        TempFreeBlockHead = TempFreeBlockTail = Current;
                    } else {
                        TempFreeBlockTail->SetNextBlock(Current);
                        TempFreeBlockTail = TempFreeBlockTail->GetNextBlock();
                    }
                }
                else {
                    MemoryBlockAllocator::Destroy(&Current);
                }
            }
            else {
                Last = Current;
            }
            Current = Last -> GetNextBlock();
        }

        if (nullptr == TempFreeBlockHead) {
            return;
        }

        Last -> SetNextBlock(TempFreeBlockHead);
        CurrentBlock = HeadBlock;
    };

    void Pool::Reset() {

        int Residual = DefaultPoolResidual;

        MemoryBlockAllocator *TempMemBlock = HeadBlock, *NextMemBlock, *NewMemBlockList = nullptr;

        while (TempMemBlock != nullptr) {

            NextMemBlock = TempMemBlock -> GetNextBlock();

            if (Residual -- > 0) {
                TempMemBlock -> Reset();
                TempMemBlock -> SetNextBlock(NewMemBlockList);
                NewMemBlockList = TempMemBlock;
            }
            else {
                MemoryBlockAllocator::Destroy(&TempMemBlock);
            }

            TempMemBlock = NextMemBlock;
        }
        CurrentBlock = HeadBlock = NewMemBlockList;
    }

    Pool::~Pool() {

        MemoryBlockAllocator *TempMemBlock = HeadBlock, *Next = nullptr;

        while (TempMemBlock != nullptr) {
            Next = TempMemBlock -> GetNextBlock();
            MemoryBlockAllocator::Destroy(&TempMemBlock);
            TempMemBlock = Next;
        }
        HeadBlock = CurrentBlock = nullptr;
    }
}
