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
    Lock.Lock();

    if (RecycleSentinel.IsEmpty()) {
        Ret = BufferMemoryBlock::Build(BufferMemoryBlockSize);
    }
    else {
        RecycleSize -= 1;
        Ret = (BufferMemoryBlock *)RecycleSentinel.GetHead();
        Ret->Detach();
    }

    Lock.Unlock();
    return Ret;
}

void BufferMemoryBlockRecycler::Put(BufferMemoryBlock *Item) {

    Lock.Lock();

    if (RecycleSize >= RecycleMaxSize) {
        BufferMemoryBlock::Destroy(&Item);
    }
    else {
        RecycleSize += 1;
        Item->Reset();
        RecycleSentinel.Append(Item);
    }

    Lock.Unlock();
}
