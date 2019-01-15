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

    MemoryBlockAllocator *Current, *Next, *NewCurrent;

    if (HeadBlock == nullptr) {
        return;
    }

    Current = HeadBlock;
    HeadBlock = nullptr;

    while(Current != nullptr) {

        Next = Current->GetNextBlock();
        Current->SetNextBlock(nullptr);

        if (Current->IsFreeBlock()) {
            MemoryBlockAllocator::Destroy(Current);
        } else {

            if (HeadBlock != nullptr) {
                NewCurrent->SetNextBlock(Current);
                NewCurrent = NewCurrent->GetNextBlock();
            } else {
                HeadBlock = NewCurrent = Current;
            }
        }
        Current = Next;
    }
};

void Pool::Reset() {

    // free all memory used

    MemoryBlockAllocator *NextBlock;

    while (HeadBlock != nullptr) {

        NextBlock = HeadBlock->GetNextBlock();
        HeadBlock->SetNextBlock(nullptr);
        MemoryBlockAllocator::Destroy(HeadBlock);
        HeadBlock = NextBlock;
    }

    CurrentBlock = nullptr; AllocateRound = 0;
}

Pool::~Pool() {
    Reset();
}
