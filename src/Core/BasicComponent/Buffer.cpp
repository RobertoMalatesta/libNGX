#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

static inline BufferMemoryBlock *AquireBlock(BufferMemoryBlockRecycleBin *R, size_t Size) {

    BufferMemoryBlock *Ret = nullptr;

    if (R == nullptr) {
        BufferMemoryBlock::Build(Ret, Size);
    } else {
        Ret = R->Get();
    }

    return Ret;
}

static inline void RecycleBlock(BufferMemoryBlockRecycleBin *R, BufferMemoryBlock *B) {

    if (R == nullptr) {
        BufferMemoryBlock::Destroy(B);
    } else {
        R->Put(B);
    }
}


Buffer::~Buffer() {

    BufferMemoryBlock *TempBlock, *NextBlock;

    TempBlock = HeadBlock;

    HeadBlock = nullptr;

    while (TempBlock != nullptr) {
        NextBlock = TempBlock->GetNextBlock();
        RecycleBlock(RecycleBin, TempBlock);
        TempBlock = NextBlock;
    }
}

RuntimeError Buffer::WriteDataToBuffer(u_char *PointerToData, size_t DataLength) {

    size_t CurrentBlockFreeSize;
    BufferMemoryBlock *TempBufferBlock, *WriteBlock;

    WriteBlock = AddressToMemoryBlock(WriteCursor.Position);

    for (;;) {

        CurrentBlockFreeSize = WriteBlock->End - WriteCursor.Position;

        if (DataLength > CurrentBlockFreeSize) {

            TempBufferBlock = AquireBlock(RecycleBin, BlockSize);

            if (TempBufferBlock == nullptr) {
                ReadCursor > WriteCursor;
                return {ENOMEM, "No enough memory!"};
            }

            memcpy(WriteCursor.Position, PointerToData, CurrentBlockFreeSize);

            PointerToData += CurrentBlockFreeSize;
            DataLength -= CurrentBlockFreeSize;

            WriteBlock->SetNextBlock(TempBufferBlock);
            WriteCursor.Position = WriteBlock->Start;
        } else {
            memcpy(WriteCursor.Position, PointerToData, DataLength);
            WriteCursor.Position += DataLength;
            break;
        }
    }

    ReadCursor > WriteCursor;
    return {0};
}

Buffer &Buffer::operator<<(BoundCursor &BC) &{
    ReadCursor = BC;
    return *this;
}

Buffer &Buffer::operator>>(BoundCursor &BC) &{
    BC = ReadCursor;
    return *this;
}

RuntimeError Buffer::WriteConnectionToBuffer(Connection *C) {

    int SocketFd = C->GetSocketFD();
    u_char *PointerToData;
    size_t ReadLength;
    ssize_t RecievedSize;
    BufferMemoryBlock *TempBlock, *WriteBlock;

    WriteBlock = AddressToMemoryBlock(WriteCursor.Position);

    while (true) {

        PointerToData = WriteCursor.Position;
        ReadLength = WriteBlock->End - PointerToData;

        if (ReadLength == 0) {

            if (RecycleBin == nullptr) {
                BufferMemoryBlock::Build(TempBlock, BlockSize);
            } else {
                TempBlock = RecycleBin->Get();
            }

            if (TempBlock == nullptr) {
                return {ENOMEM, "Can not allocate BufferMemoryBlock when recv()"};
            }
            WriteBlock->SetNextBlock(TempBlock);
            WriteBlock = TempBlock;
            PointerToData = WriteCursor.Position = WriteBlock->Start;
            ReadLength = WriteBlock->End - PointerToData;
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
    ReadCursor > WriteCursor;
    return {0};
}

void Buffer::Reset() {

    BufferMemoryBlock *TempBlock, *NextBlock, *WriteBlock;

    TempBlock = HeadBlock;
    WriteBlock = AddressToMemoryBlock(WriteCursor.Position);

    while (TempBlock != WriteBlock && TempBlock != nullptr) {

        NextBlock = TempBlock->GetNextBlock();
        TempBlock->Reset();

        if (RecycleBin == nullptr) {
            BufferMemoryBlock::Destroy(TempBlock);
        } else {
            RecycleBin->Put(TempBlock);
        }

        TempBlock = NextBlock;
    }

    if (WriteBlock != nullptr) {
        WriteBlock->Reset();
        WriteCursor.Position = WriteBlock->Start;
        ReadCursor < WriteCursor > WriteCursor;
    }
}

void Buffer::GC() {

    BufferMemoryBlock *TempBlock, *NextBlock, *ReadBlock;

    TempBlock = HeadBlock, NextBlock = TempBlock->NextBlock;
    ReadBlock = AddressToMemoryBlock(WriteCursor.Position);

    while (NextBlock != nullptr && NextBlock != ReadBlock) {

        if (NextBlock->RefCount() == 0) {
            TempBlock->SetNextBlock(NextBlock->GetNextBlock());

            if (RecycleBin == nullptr) {
                BufferMemoryBlock::Destroy(NextBlock);
            } else {
                RecycleBin->Put(NextBlock);
            }

            NextBlock = TempBlock->GetNextBlock();
        } else {
            TempBlock = NextBlock;
            NextBlock = TempBlock->GetNextBlock();
        }
    }

    if (HeadBlock->RefCount() == 0) {
        NextBlock = HeadBlock;
        HeadBlock = HeadBlock->GetNextBlock();
        NextBlock->Reset();
        RecycleBlock(RecycleBin, NextBlock);
    }
}
