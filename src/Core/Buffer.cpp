#include "Core/Core.h"

using namespace ngx::Core;

static inline BufferMemoryBlock *AquireBlock (BufferMemoryBlockRecycler *R, size_t Size) {

    if (R == nullptr) {
        return BufferMemoryBlock::Build(Size);
    } else {
        return R->Get();
    }
}

static inline void RecycleBlock (BufferMemoryBlockRecycler *R, BufferMemoryBlock *B) {

    if (R == nullptr) {
        BufferMemoryBlock::Destroy(&B);
    } else {
        R->Put(B);
    }
}

void BufferCursor::GetReference() {
    Block->GetReference();
}

void BufferCursor::PutReference() {
    Block->PutReference();
}

void BufferRange::GetReference() {
    BufferMemoryBlock *TempBlock;

    TempBlock = LeftBound.Block;

    while (TempBlock != RightBound.Block) {
        TempBlock->GetReference();
        TempBlock = TempBlock->GetNextBlock();
    }

    RightBound.Block->GetReference();
}


void BufferRange::PutReference() {
    BufferMemoryBlock *TempBlock;

    TempBlock = LeftBound.Block;

    while (TempBlock != RightBound.Block) {
        TempBlock->PutReference();
        TempBlock = TempBlock->GetNextBlock();
    }

    RightBound.Block->PutReference();
}

Buffer::Buffer(BufferMemoryBlockRecycler *Recycler, size_t BlockSize) {

    if (Recycler != nullptr && Recycler->GetBlockSize() == BlockSize) {
        this->Recycler = Recycler;
    }

    HeadBlock = AquireBlock(Recycler, BlockSize);

    if (HeadBlock == nullptr) {
        return;
    }

    this->BlockSize = BlockSize;
    ReadCursor.Block = WriteCursor.Block = HeadBlock;
    ReadCursor.Position = WriteCursor.Position = HeadBlock->Start;
}

Buffer::Buffer(Buffer &Copy) {
    Recycler = Copy.Recycler;
    HeadBlock = Copy.HeadBlock;
    ReadCursor = ReadCursor;
    WriteCursor = WriteCursor;
    Lock.Unlock();
}

Buffer::~Buffer() {

    BufferMemoryBlock *TempBlock, *NextBlock;

    TempBlock = HeadBlock;

    while (TempBlock != nullptr) {

        NextBlock = TempBlock->GetNextBlock();

        RecycleBlock(Recycler, TempBlock);

        TempBlock = NextBlock;
    }

    ReadCursor.Position = nullptr;
    ReadCursor.Block = nullptr;
    WriteCursor = ReadCursor;
}

RuntimeError Buffer::WriteDataToBuffer(u_char *PointerToData, size_t DataLength) {

    size_t CurrentBlockFreeSize;
    BufferMemoryBlock *TempBufferBlock, *WriteBlock = WriteCursor.Block;

    Lock.Lock();

    for (;;) {

        CurrentBlockFreeSize = WriteBlock->End - WriteCursor.Position;

        if (DataLength > CurrentBlockFreeSize) {

            TempBufferBlock = AquireBlock(Recycler, BlockSize);

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

RuntimeError Buffer::WriteConnectionToBuffer(Connection *C) {

    int SocketFd = C->GetSocketFD();
    u_char *PointerToData;
    size_t ReadLength;
    ssize_t RecievedSize;
    BufferMemoryBlock *TempBlock;

    while (true) {

        PointerToData = WriteCursor.Position;
        ReadLength = WriteCursor.Block->End - PointerToData;

        if (ReadLength == 0) {

            if (Recycler == nullptr) {
                TempBlock = BufferMemoryBlock::Build(BlockSize);
            }
            else {
                TempBlock = Recycler->Get();
            }

            if (TempBlock == nullptr) {
                return RuntimeError(ENOMEM, "Can not allocate BufferMemoryBlock when recv()");
            }
            WriteCursor.Block->SetNextBlock(TempBlock);
            WriteCursor.Block = TempBlock;
            PointerToData = WriteCursor.Position = WriteCursor.Block->Start;
            ReadLength = WriteCursor.Block->End - PointerToData;
        }

        RecievedSize = recv(SocketFd, PointerToData, ReadLength, 0);

        if (RecievedSize == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            else {
                return RuntimeError(errno, "Failed to read from socket!");
            }
        }
        else if (RecievedSize > 0) {
            WriteCursor.Position += RecievedSize;
        }
        else {
            break;
        }
    }

    return RuntimeError(0);
}

void Buffer::Reset() {

    BufferMemoryBlock *TempBlock, *NextBlock;

    TempBlock = HeadBlock;

    while (TempBlock != WriteCursor.Block && TempBlock != nullptr) {

        NextBlock = TempBlock->GetNextBlock();
        TempBlock->Reset();

        if (Recycler == nullptr) {
            BufferMemoryBlock::Destroy(&TempBlock);
        }
        else {
            Recycler->Put(TempBlock);
        }

        TempBlock = NextBlock;
    }

    WriteCursor.Block->Reset();
    HeadBlock = WriteCursor.Block;
    WriteCursor.Position = HeadBlock->Start;
    ReadCursor = WriteCursor;
}

void Buffer::GC() {

    BufferMemoryBlock *TempBlock, *NextBlock;

    TempBlock = HeadBlock, NextBlock = TempBlock->NextBlock;

    while (NextBlock != nullptr && NextBlock != ReadCursor.Block) {

        if (NextBlock->GetReference() == 0) {
            TempBlock->SetNextBlock(NextBlock->GetNextBlock());

            if (Recycler == nullptr) {
                BufferMemoryBlock::Destroy(&NextBlock);
            }
            else {
                Recycler->Put(NextBlock);
            }

            NextBlock = TempBlock->GetNextBlock();
        }
        else {
            TempBlock = NextBlock;
            NextBlock = TempBlock->GetNextBlock();
        }
    }

    if (HeadBlock->GetReference() == 0) {
        NextBlock = HeadBlock;
        HeadBlock = HeadBlock->GetNextBlock();
        NextBlock->Reset();
        RecycleBlock(Recycler, NextBlock);
    }
}
