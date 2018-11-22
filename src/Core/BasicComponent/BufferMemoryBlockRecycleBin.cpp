#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;


BufferMemoryBlockRecycleBin::BufferMemoryBlockRecycleBin(
        size_t BufferMemoryBlockSize,
        uint64_t RecycleBinSize) :
        RecycleBin(RecycleBinSize) {
    this->BufferMemoryBlockSize = BufferMemoryBlockSize;
}

BufferMemoryBlockRecycleBin::~BufferMemoryBlockRecycleBin() {

    BufferMemoryBlock *TempBlock;

    while (!RecycleSentinel.IsEmpty()) {
        TempBlock = (BufferMemoryBlock *) RecycleSentinel.GetNext();
        TempBlock->Detach();
        BufferMemoryBlock::Destroy(TempBlock);
    }
}

BufferMemoryBlock *BufferMemoryBlockRecycleBin::Get() {

    BufferMemoryBlock *Ret;

    LockGuard LockGuard(&Lock);

    if (RecycleSentinel.IsEmpty()) {
        BufferMemoryBlock::Build(Ret, BufferMemoryBlockSize);
    } else {
        RecycleSize -= 1;
        Ret = (BufferMemoryBlock *) RecycleSentinel.GetNext();
        Ret->Detach();
    }

    return Ret;
}

void BufferMemoryBlockRecycleBin::Put(BufferMemoryBlock *Item) {

    LockGuard LockGuard(&Lock);

    if (RecycleSize >= RecycleMaxSize) {
        BufferMemoryBlock::Destroy(Item);
    } else {
        RecycleSize += 1;
        Item->Reset();
        RecycleSentinel.Append(Item);
    }
}
