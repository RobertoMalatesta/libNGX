#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

Pool::Pool(size_t BlockSize) {

    if (BlockSize < PAGE_SIZE) {
        BlockSize = PAGE_SIZE;
        //[WARNING] Block Size should big than page size
    }

    this->BlockSize = BlockSize;
    CurrentBlock = HeadBlock = nullptr;
}

Pool::Pool(Pool &Copy) {
    BlockSize = Copy.BlockSize;
    HeadBlock = Copy.HeadBlock;
    CurrentBlock = Copy.CurrentBlock;
}

void *Pool::Allocate(size_t Size) {

    void *ret = nullptr;
    MemoryBlockAllocator *TempAllocator = nullptr;

    if (HeadBlock == nullptr && MemoryBlockAllocator::Build(HeadBlock, BlockSize) == 0) {
        CurrentBlock = HeadBlock;
    } else if (HeadBlock == nullptr || Size == 0) {
        return nullptr;
    }

    if (Size > BlockSize - 2 * sizeof(MemoryBlockAllocator)) {
        return malloc(Size);
    } else {
        ret = CurrentBlock->Allocate(Size);

        if (ret != nullptr) {
            return ret;
        } else if (CurrentBlock->GetNextBlock() != nullptr) {
            CurrentBlock = CurrentBlock->GetNextBlock();
        } else {

            MemoryBlockAllocator *NewBlock;

            if (MemoryBlockAllocator::Build(NewBlock, BlockSize) == 0) {
                CurrentBlock->SetNextBlock(NewBlock);
                CurrentBlock = CurrentBlock->GetNextBlock();
            } else {
                return nullptr;
            }
        }

        if ((++AllocateRound) % POOL_RECYCLE_ROUND == 0) {
            GC();
        }

        return CurrentBlock->Allocate(Size);
    }
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


    if (HeadBlock == nullptr) {
        return;
    }

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

    // free all memory used

    MemoryBlockAllocator *TempMemBlock = HeadBlock, *NextMemBlock;

    AllocateRound = 0;

    while (TempMemBlock != nullptr) {

        NextMemBlock = TempMemBlock->GetNextBlock();

        MemoryBlockAllocator::Destroy(TempMemBlock);

        TempMemBlock = NextMemBlock;
    }
    HeadBlock = CurrentBlock = nullptr;
}

Pool::~Pool() {
    Reset();
}
