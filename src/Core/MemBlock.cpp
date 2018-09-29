#include "Core/Core.h"

namespace ngx::Core {

    MemoryBlock::MemoryBlock(size_t Size) {
        PointerToHead = (u_char *)this + sizeof(MemoryBlock);
        TotalSize = Size - sizeof(MemoryBlock);
        Reset();
        Magic = (void *)this;
    }

    MemoryBlock::~MemoryBlock() {
        Magic = nullptr;
        PointerToData = PointerToHead = nullptr;
    }

    MemoryBlock* MemoryBlock::Build(size_t Size) {

        void *TempPointer = valloc(Size);

        if (nullptr == TempPointer) {
            return nullptr;
        }

        return new(TempPointer) MemoryBlock(Size);
    }

    void MemoryBlock::Destroy(MemoryBlock **PointerToBlock) {
        if (nullptr == PointerToBlock || nullptr == *PointerToBlock) {
            return;
        }

        free((void*)*PointerToBlock);
        *PointerToBlock = nullptr;
    }

    MemoryBlock* MemoryBlock::AddressToMemoryBlock(void *Address, size_t Size) {

        MemoryBlock * MemBlk;
        MemBlk = (MemoryBlock *)( (size_t)Address & ~(Size - 1));

        if (nullptr != MemBlk && MemBlk -> Magic ==(void *) MemBlk) {
            return MemBlk;
        }

        return nullptr;
    }

    bool MemoryBlock::IsInBlock(void *Address) {
        return (Address >= PointerToHead && Address < ((u_char *)PointerToData));
    }

    MemoryBlockAllocator::MemoryBlockAllocator(size_t Size): MemoryBlock(Size) {
        PointerToHead = (u_char *)this + sizeof(MemoryBlockAllocator);
        TotalSize = Size - sizeof(MemoryBlockAllocator);
        Reset();
        Magic = (void *)this;
    }

    MemoryBlockAllocator::~MemoryBlockAllocator() {
        MemoryBlock::~MemoryBlock();
    }

    MemoryBlockAllocator* MemoryBlockAllocator::Build(size_t Size) {
        void *TempPointer = valloc(Size);

        if (nullptr == TempPointer) {
            return nullptr;
        }

        return new(TempPointer) MemoryBlockAllocator(Size);
    }

    void MemoryBlockAllocator::Destroy(MemoryBlockAllocator **PointerToAllocator) {
        if (nullptr == PointerToAllocator || nullptr == *PointerToAllocator) {
            return;
        }

        free((void*)*PointerToAllocator);
        *PointerToAllocator = nullptr;
    }

    void* MemoryBlockAllocator::Allocate(size_t Size) {
        void *ret = nullptr;

        if (FreeSize >= Size) {
            UseCount++;
            ret = PointerToData;
            PointerToData = (u_char *)PointerToData + Size;
            FreeSize -= Size;
        }

        return ret;
    }

    void MemoryBlockAllocator::Free(void **Pointer) {
        if (nullptr != Pointer && IsInBlock(*Pointer)) {
            *Pointer = nullptr;
            UseCount--;
            if ((UseCount < 0)) {
                //[UNREACHABLE BRANCH] BUG: Over free, will add log here later
            }
        }
    }

    BufferMemoryBlock::BufferMemoryBlock(size_t Size) : MemoryBlock(Size), Recyclable() {
        TotalSize = Size - sizeof(BufferMemoryBlock);
        FreeSize = TotalSize;
        PointerToHead = (u_char *)this + sizeof(BufferMemoryBlock);
        PointerToData = PointerToHead;
        Start = Pos = (u_char *)PointerToHead;
        End = Start + TotalSize;
    }

    BufferMemoryBlock::~BufferMemoryBlock() {
        Start = Pos = End = nullptr;
    }

    BufferMemoryBlock* BufferMemoryBlock::Build(size_t Size) {
        void *TempPointer = valloc(Size);

        if (nullptr == TempPointer) {
            return nullptr;
        }

        return new(TempPointer) BufferMemoryBlock(Size);
    }

    void BufferMemoryBlock::Destroy(BufferMemoryBlock **PointerToBlock) {

        if (nullptr == PointerToBlock || nullptr == *PointerToBlock) {
            return;
        }

        free((void*)*PointerToBlock);
        *PointerToBlock = nullptr;
    }
}
