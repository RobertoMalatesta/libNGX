#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;


BufferMemoryBlockRecycleBin::BufferMemoryBlockRecycleBin(
        size_t BufferMemoryBlockSize,
        uint64_t RecycleBinSize) :
        RecycleBin(RecycleBinSize) {
    this->BufferMemoryBlockSize = BufferMemoryBlockSize;
}

BufferMemoryBlock *BufferMemoryBlockRecycleBin::Get() {

    BufferMemoryBlock *Ret;

    SpinlockGuard LockGuard(&Lock);

    if (RecycleSentinel.IsEmpty()) {
        BufferMemoryBlock::Build(Ret, BufferMemoryBlockSize);
    } else {
        RecycleSize -= 1;
        Ret = (BufferMemoryBlock *) RecycleSentinel.GetHead();
        Ret->Detach();
    }

    return Ret;
}

void BufferMemoryBlockRecycleBin::Put(BufferMemoryBlock *Item) {

    SpinlockGuard LockGuard(&Lock);

    if (RecycleSize >= RecycleMaxSize) {
        BufferMemoryBlock::Destroy(Item);
    } else {
        RecycleSize += 1;
        Item->Reset();
        RecycleSentinel.Append(Item);
    }
}
