#include "Core/Core.h"

using namespace ngx::Core::Support;


BufferMemoryBlockCollector::BufferMemoryBlockCollector(
        size_t BlockSize,
        uint32_t CollectorSize) :
        Collector(CollectorSize), BlockSize(BlockSize) {
}

BufferMemoryBlockCollector::~BufferMemoryBlockCollector() {
    std::lock_guard<spin_lock> g(Lock);
    Reset();
}

RuntimeError BufferMemoryBlockCollector::SetCollectorSize(uint32_t Size) {

    std::lock_guard<spin_lock> g(Lock);

    Reset();
    CollectorSize = Size;
    return {0};
}

RuntimeError BufferMemoryBlockCollector::SetBlockSize(size_t Size) {

    std::lock_guard<spin_lock> g(Lock);

    if (BlockSize != Size) {
        this->BlockSize = Size;
        Reset();
    }

    return {0};
}

BufferMemoryBlock *BufferMemoryBlockCollector::Get() {

    BufferMemoryBlock *Ret = nullptr;

    std::lock_guard<spin_lock> g(Lock);

    if (CollectorSentinel.isEmpty()) {
        Ret = BufferMemoryBlock::Build(BlockSize);
    } else {

        Ret = BufferMemoryBlock::FromCollectorqnode(CollectorSentinel.next());
        Ret->ReuseItem.detach();

        CollectorSize += 1;
    }

    return Ret;
}

void BufferMemoryBlockCollector::Put(BufferMemoryBlock *Item) {

    std::lock_guard<spin_lock> g(Lock);

    if (CollectorSize <= 0) {
        BufferMemoryBlock::Destroy(Item);
    } else {

        CollectorSize -= 1;

        Item->Reset();
        CollectorSentinel.append(&Item->ReuseItem);
    }
}

void BufferMemoryBlockCollector::Reset() {

    BufferMemoryBlock *TempBlock;

    while (!CollectorSentinel.isEmpty()) {

        TempBlock = BufferMemoryBlock::FromCollectorqnode(CollectorSentinel.next());
        TempBlock->ReuseItem.detach();

        BufferMemoryBlock::Destroy(TempBlock);

        CollectorSize += 1;
    }
}