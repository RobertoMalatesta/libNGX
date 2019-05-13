#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

std::unique_ptr<MemoryPool::PoolMemoryBlock> MemoryPool::PoolMemoryBlock::newBlock(size_t BlockSize) {

    void *pointer = nullptr;

    pointer = malloc(BlockSize);

    if (pointer == nullptr) {
        return std::unique_ptr<MemoryPool::PoolMemoryBlock>(nullptr);
    }

    Byte *Start = reinterpret_cast<Byte *>(pointer);

    std::unique_ptr<MemoryPool::PoolMemoryBlock> Ret(new MemoryPool::PoolMemoryBlock());

    Ret->Start = Ret->Pos = Start;
    Ret->End = Start + BlockSize;

    return Ret;
}

bool MemoryPool::PoolMemoryBlock::isFreeBlock() {
    return RefCount == 0;
}

Byte *MemoryPool::PoolMemoryBlock::allocate(size_t Size) {

    Byte *Ret = nullptr;

    if (Pos + Size < End) {
        Ret = Pos, Pos += Size;
    }

    return Ret;
}

void MemoryPool::PoolMemoryBlock::free(void *&Pointer) {
    if (Pointer > Start && Pointer < End && RefCount > 0) {
       RefCount--;
    }
    Pointer = nullptr;
}

MemoryPool::MemoryPool(size_t BlockSize) :BlockSize(BlockSize){
    BlockList.clear();
    CurrentBlock = BlockList.begin();
}

void* MemoryPool::allocate(size_t Size) {

    Byte *Ret;

    if(Size *2 > BlockSize) {
        return malloc(Size);
    }

    for(auto &B: BlockList) {
        Ret = (*B).allocate(Size);

        if (Ret != nullptr) {
            return reinterpret_cast<void *>(Ret);
        }
    }

    auto pNewBlock = PoolMemoryBlock::newBlock(BlockSize);

    if (pNewBlock == nullptr) {
        return nullptr;
    }

    Ret = pNewBlock->allocate(Size);

    BlockList.push_back(pNewBlock);

    return reinterpret_cast<void *>(Ret);
}

void MemoryPool::free(void *&Pointer) {
}

void MemoryPool::collect() {
}
