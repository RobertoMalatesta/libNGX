#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

Pool::Pool(size_t BlockSize) {

    if (BlockSize < 1024) {
        BlockSize = 1024;
        //[WARNING] Block Size too small
    }

    this->BlockSize = BlockSize;
    MemoryBlockAllocator::Build(HeadBlock, BlockSize);
    CurrentBlock = HeadBlock;
}

Pool::Pool(Pool &Copy) {
    BlockSize = Copy.BlockSize;
    HeadBlock = Copy.HeadBlock;
    CurrentBlock = Copy.CurrentBlock;
}

void *Pool::Allocate(size_t Size) {

    void *ret = nullptr;
    MemoryBlockAllocator *TempAllocator = nullptr;

    if (Size == 0) {
        return ret;
    } else if (Size > BlockSize - 4 * sizeof(MemoryBlockAllocator)) {
        ret = malloc(Size);
    } else {
        do {
            ret = CurrentBlock->Allocate(Size);

            if (ret != nullptr) {
                break;
            }
            if (CurrentBlock->GetNextBlock() != nullptr) {
                CurrentBlock = CurrentBlock->GetNextBlock();
            } else {

                if (MemoryBlockAllocator::Build(TempAllocator, BlockSize) != 0) {
                    break;
                }
                CurrentBlock->SetNextBlock(TempAllocator);
            }
        } while (true);
    }

    if ((AllocateRound++) % POOL_RECYCLE_ROUND == 0) {
        GC();
    }

    return ret;
}

void Pool::Free(void *&pointer) {

    bool FoundInBlock = false;

    MemoryBlockAllocator *TempBlock = HeadBlock;

    if (nullptr != pointer) {

        while (nullptr != TempBlock) {
            if (TempBlock->IsInBlock(pointer)) {
                TempBlock->Free(pointer);
                FoundInBlock = true;
                break;
            }
            TempBlock = TempBlock->GetNextBlock();
        }

        if (!FoundInBlock) {
            free(pointer);
        }

        if ((AllocateRound++) % POOL_RECYCLE_ROUND == 0) {
            GC();
        }
    }
}

void Pool::GC() {

    int Residual = POOL_RESIDUAL;

    MemoryBlockAllocator *Last = HeadBlock, *Current = nullptr /*, *Next = nullptr */, *TempFreeBlockHead = nullptr, *TempFreeBlockTail = nullptr;

    Current = Last->GetNextBlock();

    if (HeadBlock->IsFreeBlock()) {
        HeadBlock->Reset();
    }

    while (Current != nullptr) {

        if (Current->IsFreeBlock()) {

            Last->SetNextBlock(Current->GetNextBlock());
            Current->SetNextBlock(nullptr);

            if (Residual > 0) {
                Residual -= 1;
                Current->Reset();
                if (nullptr == TempFreeBlockTail) {
                    TempFreeBlockHead = TempFreeBlockTail = Current;
                } else {
                    TempFreeBlockTail->SetNextBlock(Current);
                    TempFreeBlockTail = TempFreeBlockTail->GetNextBlock();
                }
            } else {
                MemoryBlockAllocator::Destroy(Current);
            }
        } else {
            Last = Current;
        }
        Current = Last->GetNextBlock();
    }

    if (nullptr == TempFreeBlockHead) {
        return;
    }

    Last->SetNextBlock(TempFreeBlockHead);
    CurrentBlock = HeadBlock;
};

void Pool::Reset() {

    int Residual = POOL_RESIDUAL;

    MemoryBlockAllocator *TempMemBlock = HeadBlock, *NextMemBlock, *NewMemBlockList = nullptr;

    while (TempMemBlock != nullptr) {

        NextMemBlock = TempMemBlock->GetNextBlock();

        if (Residual-- > 0) {
            TempMemBlock->Reset();
            TempMemBlock->SetNextBlock(NewMemBlockList);
            NewMemBlockList = TempMemBlock;
        } else {
            MemoryBlockAllocator::Destroy(TempMemBlock);
        }

        TempMemBlock = NextMemBlock;
    }
    CurrentBlock = HeadBlock = NewMemBlockList;
}

Pool::~Pool() {

    MemoryBlockAllocator *TempMemBlock = HeadBlock, *Next = nullptr;

    while (TempMemBlock != nullptr) {
        Next = TempMemBlock->GetNextBlock();
        MemoryBlockAllocator::Destroy(TempMemBlock);
        TempMemBlock = Next;
    }
    HeadBlock = CurrentBlock = nullptr;
}
