#include "Core/Core.h"

using namespace ngx::Core::Support;

Pool::PoolMemoryBlock *Pool::PoolMemoryBlock::newBlock(size_t Size) {

    void *pMem = nullptr;
    u_char *pDataStart = nullptr, *pDataEnd;
    size_t AllocateSize = Size + sizeof(PoolMemoryBlock);

    if (posix_memalign(&pMem, 8, AllocateSize) != 0 || pMem == nullptr) {
        return nullptr;
    }

    auto newBlock = new(pMem) PoolMemoryBlock();

    pDataStart = reinterpret_cast<u_char *>(pMem) + sizeof(PoolMemoryBlock);
    pDataEnd = reinterpret_cast<u_char *>(pMem) + AllocateSize;

    newBlock->Start = newBlock->Pos = pDataStart;
    newBlock->End = pDataEnd;

    return newBlock;
};

Byte *Pool::PoolMemoryBlock::allocate(size_t Size) {

    void *Ret = nullptr;

    if (Pos + Size < End) {
        RefCount++, Ret = Pos, Pos += Size;
    }
    return reinterpret_cast<Byte *>(Ret);
}

void Pool::PoolMemoryBlock::free(Byte *&Pointer) {
    if (nullptr != Pointer && IsInBlock(Pointer)) {
        Pointer = nullptr, RefCount--;
    }
}

Pool::Pool() {
    CurrentBlock = HeadBlock = nullptr;
}

void *Pool::allocate(size_t Size) {

    void *ret = nullptr;

    if (HeadBlock == nullptr) {

        HeadBlock = PoolMemoryBlock::newBlock(BLOCK_SIZE);

        if (HeadBlock != nullptr) {
            CurrentBlock = HeadBlock;
        }
    } else if (HeadBlock == nullptr || Size == 0) {
        return nullptr;
    }

    if (Size > BLOCK_SIZE - 2 * sizeof(PoolMemoryBlock)) {
        return malloc(Size);
    } else {

        ret = CurrentBlock->allocate(Size);

        if (ret != nullptr) {
            return ret;
        } else if (CurrentBlock->getNextBlock() != nullptr) {
            CurrentBlock = CurrentBlock->getNextBlock();
        } else {

            PoolMemoryBlock *NewBlock;

            NewBlock = PoolMemoryBlock::newBlock(BLOCK_SIZE);

            if (NewBlock != nullptr) {
                CurrentBlock->setNextBlock(NewBlock);
                CurrentBlock = CurrentBlock->getNextBlock();
            } else {
                return nullptr;
            }
        }

        if ((++AllocateRound) % POOL_COLLECT_ROUND == 0) { collect(); }

        return CurrentBlock->allocate(Size);
    }
}

void Pool::free(void *&pointer) {

    bool FoundInBlock = false;

    PoolMemoryBlock *TempBlock = HeadBlock;

    if (nullptr != pointer) {

        while (nullptr != TempBlock) {
            if (TempBlock->IsInBlock(pointer)) {
                TempBlock->free(reinterpret_cast<Byte *&>(pointer));
                FoundInBlock = true;
                break;
            }
            TempBlock = TempBlock->getNextBlock();
        }

        if (!FoundInBlock) {
            free(pointer);
        }

        pointer = nullptr;
    }
}

void Pool::collect() {

    PoolMemoryBlock *Current, *Next, *NewCurrent = nullptr;

    if (HeadBlock == nullptr) {
        return;
    }

    Current = HeadBlock, HeadBlock = nullptr;

    while (Current != nullptr) {

        Next = Current->getNextBlock();
        Current->setNextBlock(nullptr);

        if (Current->IsFreeBlock()) {
            delete Current;
        } else {
            if (HeadBlock == nullptr) {
                HeadBlock = Current;
            } else {
                NewCurrent->setNextBlock(Current);
            }
            NewCurrent = Current;
        }
        Current = Next;
    }

    if (NewCurrent != nullptr) {
        NewCurrent->setNextBlock(nullptr);
    }

    CurrentBlock = HeadBlock;
};

void Pool::reset() {

    PoolMemoryBlock *NextBlock;

    while (HeadBlock != nullptr) {

        NextBlock = HeadBlock->getNextBlock();
        HeadBlock->setNextBlock(nullptr);
        delete HeadBlock, HeadBlock = NextBlock;
    }

    CurrentBlock = nullptr;
    AllocateRound = 0;
}

Pool::~Pool() {
    reset();
}
