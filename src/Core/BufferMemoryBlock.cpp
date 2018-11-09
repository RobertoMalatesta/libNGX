#include "Core/Core.h"

using namespace ngx::Core;

BufferMemoryBlock::BufferMemoryBlock(size_t Size) : BasicMemoryBlock(Size), Recyclable() {
    TotalSize = Size - sizeof(BufferMemoryBlock);
    FreeSize = TotalSize;
    PointerToHead = (u_char *) this + sizeof(BufferMemoryBlock);
    PointerToData = PointerToHead;
    Start = (u_char *) PointerToHead;
    End = Start + TotalSize;
}

BufferMemoryBlock::~BufferMemoryBlock() {
    Start = End = nullptr;
}

int BufferMemoryBlock::Build(BufferMemoryBlock* &Item, size_t Size) {

    void *TempPointer = valloc(Size);

    if (nullptr == TempPointer) {
        return errno;
    }

    Item = new(TempPointer) BufferMemoryBlock(Size);

    return 0;
}

int BufferMemoryBlock::Destroy(BufferMemoryBlock* &Item) {

    if (nullptr == Item) {
        return 0;
    }

    free((void *) Item);
    Item = nullptr;

    return 0;
}

void BufferMemoryBlock::Reset() {
    PointerToHead = (u_char *) this + sizeof(BufferMemoryBlock);
    PointerToData = PointerToHead;
    Start = (u_char *) PointerToHead;
    End = Start + TotalSize;
    NextBlock = nullptr;
}
