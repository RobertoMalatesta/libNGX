#include "Core/Core.h"

using namespace ngx::Core;

Buffer::Buffer(BufferMemoryBlockRecycler *Recycler, size_t BlockSize) {

    if (Recycler != nullptr && Recycler->GetBlockSize() == BlockSize) {
        this->Recycler = Recycler;
    }

    HeadBlock = BufferMemoryBlock::Build(BlockSize);

    if (HeadBlock == nullptr) {
        return;
    }
    this->BlockSize = BlockSize;
    CurrentBlock = HeadBlock;

    ReadCursor.Block = WriteCursor.Block = HeadBlock;
    ReadCursor.Position = WriteCursor.Position = HeadBlock->Start;
}

RuntimeError Buffer::WriteDataToBuffer(u_char *PointerToData, size_t DataLength) {

    size_t CurrentBlockFreeSize;
    BufferMemoryBlock *TempBufferBlock, *WriteBlock = WriteCursor.Block;

    Lock.Lock();

    for (;;) {

        CurrentBlockFreeSize = WriteBlock->End - WriteCursor.Position;

        if (DataLength > CurrentBlockFreeSize) {
            if (Recycler == nullptr) {

                TempBufferBlock = BufferMemoryBlock::Build(BlockSize);
            }
            else {
                TempBufferBlock = Recycler->Get();
            }

            if (TempBufferBlock == nullptr) {
                Lock.Unlock();
                return RuntimeError(ENOMEM, "No enough memory!");
            }

            memcpy(WriteCursor.Position, PointerToData, CurrentBlockFreeSize);

            PointerToData += CurrentBlockFreeSize;
            DataLength -= CurrentBlockFreeSize;


            WriteBlock -> SetNextBlock(TempBufferBlock);
            WriteBlock = WriteCursor.Block = TempBufferBlock;
            WriteCursor.Position = WriteBlock->Start;
        }
        else {
            memcpy(WriteCursor.Position, PointerToData, DataLength);
            WriteCursor.Position = (WriteBlock->Pos += DataLength);

            break;
        }
    }

    Lock.Unlock();

    return RuntimeError(0);
}

u_char Buffer::ReadByte() {

    if (ReadCursor.Block == WriteCursor.Block && ReadCursor.Position == WriteCursor.Position) {
        return 0;
    }

    Lock.Lock();

    if (ReadCursor.Position > ReadCursor.Block->End) {
        ReadCursor.Block = ReadCursor.Block->GetNextBlock();
        ReadCursor.Position = ReadCursor.Block->Start;
    }

    u_char Ret = *(ReadCursor.Position ++);
    Lock.Unlock();

    return Ret;
}

void Buffer::Reset() {

}

BufferMemoryBlockRecycler::BufferMemoryBlockRecycler(
        size_t BufferMemoryBlockSize,
        uint64_t RecyclerSize,
        MemAllocator *Allocator ) :
        Recycler(RecyclerSize, Allocator){
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