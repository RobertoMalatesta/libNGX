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

bool Cursor::operator~() {
    return Block == nullptr || Position == nullptr;
}

u_char Cursor::operator*() {
    return this->operator~()? (u_char)'\0': *Position;
}

bool Cursor::operator>(Cursor &Right) {
    return Block == Right.Block && Position >= Right.Position;
}

Cursor& Cursor::operator=(Cursor const &Right) {
    Block = Right.Block, Position = Right.Position;
    return *this;
}

void Cursor::GetReference() {
    Block->GetReference();
}

void Cursor::PutReference() {
    Block->PutReference();
}

BoundCursor BoundCursor::operator+(size_t Size) {

    BoundCursor Ret = *this;

    while (Size > 0) {
        if ((Position + Size) < Block->End) {
            if (this->operator > (Bound)) {
                Block = nullptr, Position = nullptr;
            }
            else {
                Position += Size;
            }
        } else {
            Size -= (Block->End - Position);
            Block = Block->GetNextBlock();
            Position = Block->Start;
        }
    }
    return Ret;
}

BoundCursor BoundCursor::operator+=(size_t Size) {

    BoundCursor Ret = *this;
    while (Size > 0) {
        if ((Position + Size) < Block->End) {
            if (this->operator > (Bound)) {
                Block = nullptr, Position = nullptr;
            }
            else {
                Position += Size;
            }
        } else {
            Size -= (Block->End - Position);
            Block = Block->GetNextBlock();
            Position = Block->Start;
        }
    }
    return Ret;
}

const BoundCursor BoundCursor::operator++(int) {
    return this->operator+=(1);
}

u_char BoundCursor::operator[](uint16_t Offset) {
    BoundCursor Target = operator+(Offset);
    return *Target;
}

BoundCursor& BoundCursor::operator<<(ngx::Core::Cursor LeftBound) {
    static_cast<Cursor *>(this)->operator=(LeftBound);
    return *this;
}

BoundCursor& BoundCursor::operator>>(ngx::Core::Cursor RightBound) {
    Bound = RightBound;
    return *this;
}

void Range::GetReference() {
    BufferMemoryBlock *TempBlock;

    TempBlock = LeftBound.Block;

    while (TempBlock != RightBound.Block) {
        TempBlock->GetReference();
        TempBlock = TempBlock->GetNextBlock();
    }

    RightBound.Block->GetReference();
}


void Range::PutReference() {
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
}

RuntimeError Buffer::WriteDataToBuffer(u_char *PointerToData, size_t DataLength) {

    size_t CurrentBlockFreeSize;
    BufferMemoryBlock *TempBufferBlock, *WriteBlock = WriteCursor.Block;

    SpinlockGuard LockGuard(&Lock);

    for (;;) {

        CurrentBlockFreeSize = WriteBlock->End - WriteCursor.Position;

        if (DataLength > CurrentBlockFreeSize) {

            TempBufferBlock = AquireBlock(Recycler, BlockSize);

            if (TempBufferBlock == nullptr) {
                return {ENOMEM, "No enough memory!"};
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

    return {0};
}

Buffer& Buffer::operator<<(BoundCursor &BC) & {
    ReadCursor = BC;
    return *this;
}

Buffer& Buffer::operator>>(BoundCursor &BC) & {
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
                return {ENOMEM, "Can not allocate BufferMemoryBlock when recv()"};
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
                return {errno, "Failed to read from socket!"};
            }
        } else if (RecievedSize > 0) {
            WriteCursor.Position += RecievedSize;
        } else {
            break;
        }
    }

    ReadCursor >> WriteCursor;

    return {0};
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
        ReadCursor.Position = ReadCursor.Block->Start;
        ReadCursor >> WriteCursor;
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
