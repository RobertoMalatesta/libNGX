#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;


BufferMemoryBlockCollector::BufferMemoryBlockCollector(
        size_t BlockSize,
        uint32_t CollectorSize) :
        Collector(CollectorSize), BlockSize(BlockSize) {
}

BufferMemoryBlockCollector::~BufferMemoryBlockCollector() {

    LockGuard LockGuard(&Lock);
    Reset();
}

RuntimeError BufferMemoryBlockCollector::SetCollectorSize(uint32_t Size) {

    LockGuard LockGuard(&Lock);

    Reset();
    CollectorSize = Size;
    return {0};
}

RuntimeError BufferMemoryBlockCollector::SetBlockSize(size_t Size) {

    LockGuard LockGuard(&Lock);

    if (BlockSize != Size) {
        this->BlockSize = Size;
        Reset();
    }

    return {0};
}

BufferMemoryBlock *BufferMemoryBlockCollector::Get() {

    BufferMemoryBlock *Ret = nullptr;

    LockGuard LockGuard(&Lock);

    if (CollectorSentinel.IsEmpty()) {
        BufferMemoryBlock::Build(Ret, BlockSize);
    } else {

        Ret = BufferMemoryBlock::FromCollectorQueue(CollectorSentinel.GetNext());
        Ret->ReuseItem.Detach();

        CollectorSize += 1;
    }

    return Ret;
}

void BufferMemoryBlockCollector::Put(BufferMemoryBlock *Item) {

    LockGuard LockGuard(&Lock);

    if (CollectorSize <= 0) {
        BufferMemoryBlock::Destroy(Item);
    } else {

        CollectorSize -= 1;

        Item->Reset();
        CollectorSentinel.Append(&Item->ReuseItem);
    }
}

void BufferMemoryBlockCollector::Reset() {

    BufferMemoryBlock *TempBlock;

    while (!CollectorSentinel.IsEmpty()) {

        TempBlock = BufferMemoryBlock::FromCollectorQueue(CollectorSentinel.GetNext());
        TempBlock->ReuseItem.Detach();

        BufferMemoryBlock::Destroy(TempBlock);

        CollectorSize += 1;
    }
}