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

    ReadBlock = WriteBlock = HeadBlock;
    ReadPosition = WritePosition = HeadBlock->Start;
}

RuntimeError Buffer::WriteDataToBuffer(u_char *PointerToData, size_t DataLength) {

    size_t CurrentBlockFreeSize;
    BufferMemoryBlock *TempBufferBlock;

    Lock.Lock();

    for (;;) {

        CurrentBlockFreeSize = WriteBlock->End - WriteBlock->Pos;

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

            memcpy(WriteBlock->Pos, PointerToData, CurrentBlockFreeSize);

            PointerToData += CurrentBlockFreeSize;
            DataLength -= CurrentBlockFreeSize;

            WriteBlock->Pos = WriteBlock->End;

            WriteBlock -> SetNextBlock(TempBufferBlock);
            WriteBlock = TempBufferBlock;
            WritePosition = WriteBlock->Start;
        }
        else {
            memcpy(WriteBlock->Pos, PointerToData, DataLength);
            WriteBlock->Pos += DataLength;
            break;
        }
    }

    Lock.Unlock();

    return RuntimeError(0);
}

u_char Buffer::ReadByte() {

    if (ReadBlock == WriteBlock && ReadPosition == WritePosition) {
        return 0;
    }

    Lock.Lock();

    if (ReadPosition > ReadBlock->End) {
        ReadBlock = ReadBlock->GetNextBlock();
        ReadPosition = ReadBlock->Start;
    }

    u_char Ret = *(ReadPosition ++);
    Lock.Unlock();

    return Ret;
}

void Buffer::Reset() {

}

BufferMemoryBlockRecycler::BufferMemoryBlockRecycler(
        size_t BufferMemoryBlockSize,
        uint64_t RecyclerSize,
        MemAllocator *Allocator ) :
        Recylcer(RecyclerSize, Allocator){
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