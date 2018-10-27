#include "Core/Core.h"

using namespace ngx::Core;

static inline BufferMemoryBlock *AquireBlock(BufferMemoryBlockRecycler *R, size_t Size) {

    if (R == nullptr) {
        return BufferMemoryBlock::Build(Size);
    } else {
        return R->Get();
    }
}

static inline void RecycleBlock(BufferMemoryBlockRecycler *R, BufferMemoryBlock *B) {

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

BufferCursor& BufferCursor::operator+=(size_t Size) {
    while (Size > 0) {
        if ((Position + Size) < Block->End) {
            if ((Block == BlockRightBound) && (Position + Size) >= PositionRightBound) {
                Block = nullptr, Position = nullptr;
            } else {
                Position += Size;
            }
        } else {
            Size -= (Block->End - Position);
            Block = Block->GetNextBlock();
            Position = Block->Start;
        }
    }
    return *this;
}

BufferCursor& BufferCursor::operator++(int) {
    return this->operator+=(1);
}

BufferCursor BufferCursor::operator+(size_t Size) {
    BufferCursor Cursor = *this;
    Cursor += Size;
    return Cursor;
}

u_char BufferCursor::operator*() {
    if (Position != nullptr && Block != nullptr) {
        return *Position;
    }
    else {
        return '\0';
    }
}

u_char BufferCursor::operator[](uint16_t Offset) {


    if ((Position + Offset) < Block->End) {
        return *(Position + Offset);
    } else {
        BufferCursor Cursor = *this;
        Cursor += Offset;

        if (Cursor.Block != nullptr && Cursor.Position != nullptr) {
            return *Cursor.Position;
        }
        return  '\0';
    }
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

Buffer::~Buffer() {

    BufferMemoryBlock *TempBlock, *NextBlock;

    TempBlock = HeadBlock;

    HeadBlock = nullptr;

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


            WriteBlock->SetNextBlock(TempBufferBlock);
            WriteBlock = WriteCursor.Block = TempBufferBlock;
            WriteCursor.Position = WriteBlock->Start;
        } else {
            memcpy(WriteCursor.Position, PointerToData, DataLength);
            WriteCursor.Position = (WriteBlock->Pos += DataLength);

            break;
        }
    }

    Lock.Unlock();

    return RuntimeError(0);
}

Buffer& Buffer::operator<<(BufferCursor &BC) & {
    ReadCursor = BC;
    return *this;
}

Buffer& Buffer::operator>>(BufferCursor &BC) & {
    BC = ReadCursor;
    return *this;
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
            } else {
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
            } else {
                return RuntimeError(errno, "Failed to read from socket!");
            }
        } else if (RecievedSize > 0) {
            WriteCursor.Position += RecievedSize;
        } else {
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
        } else {
            Recycler->Put(TempBlock);
        }

        TempBlock = NextBlock;
    }

    if (WriteCursor.Block) {
        WriteCursor.Block->Reset();
        WriteCursor.Position = WriteCursor.Block->Start;
        ReadCursor = WriteCursor;
    }
}

void Buffer::GC() {

    BufferMemoryBlock *TempBlock, *NextBlock;

    TempBlock = HeadBlock, NextBlock = TempBlock->NextBlock;

    while (NextBlock != nullptr && NextBlock != ReadCursor.Block) {

        if (NextBlock->GetReference() == 0) {
            TempBlock->SetNextBlock(NextBlock->GetNextBlock());

            if (Recycler == nullptr) {
                BufferMemoryBlock::Destroy(&NextBlock);
            } else {
                Recycler->Put(NextBlock);
            }

            NextBlock = TempBlock->GetNextBlock();
        } else {
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
