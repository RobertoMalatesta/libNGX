#include "Core/Core.h"

using namespace ngx::Core;

BufferMemoryBlockRecycler::BufferMemoryBlockRecycler(
        size_t BufferMemoryBlockSize,
        uint64_t RecyclerSize) :
        Recycler(RecyclerSize){
    this -> BufferMemoryBlockSize = BufferMemoryBlockSize;
}

BufferMemoryBlock* BufferMemoryBlockRecycler::Get() {

    BufferMemoryBlock *Ret;
    
    SpinlockGuard LockGuard(&Lock);

    if (RecycleSentinel.IsEmpty()) {
        Ret = BufferMemoryBlock::Build(BufferMemoryBlockSize);
    }
    else {
        RecycleSize -= 1;
        Ret = (BufferMemoryBlock *)RecycleSentinel.GetHead();
        Ret->Detach();
    }

    return Ret;
}

void BufferMemoryBlockRecycler::Put(BufferMemoryBlock *Item) {
    
    SpinlockGuard LockGuard(&Lock);

    if (RecycleSize >= RecycleMaxSize) {
        BufferMemoryBlock::Destroy(&Item);
    }
    else {
        RecycleSize += 1;
        Item->Reset();
        RecycleSentinel.Append(Item);
    }
}
