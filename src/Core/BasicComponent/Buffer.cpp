#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

Buffer::~Buffer() {
    Reset();
}

Buffer &Buffer::operator<<(BoundCursor &BC) &{
    Cursor = BC;
    return *this;
}

Buffer &Buffer::operator>>(BoundCursor &BC) &{
    BC = Cursor;
    return *this;
}

RuntimeError Buffer::ReadConnection(Connection *C) {

    int SocketFD = C->GetSocketFD();
    u_char *PointerToData;
    size_t ReadLength;
    ssize_t RecievedSize;
    BufferMemoryBlock *TempBlock, *WriteBlock;

    if (HeadBlock == nullptr) {

        if (Collector == nullptr) {
            BufferMemoryBlock::Build(HeadBlock, BUFFER_MEMORY_BLOCK_SIZE);
        } else {
            HeadBlock = Collector->Get();
        }

        if (HeadBlock == nullptr) {
            return {ENOMEM, "can not allocate buffer block"};
        } else {
            Cursor = {this, HeadBlock->Start, HeadBlock->Start};
        }
    }

    WriteBlock = AddressToMemoryBlock(Cursor.Bound);

    while (true) {

        PointerToData = Cursor.Bound;
        ReadLength = WriteBlock->End - PointerToData;

        if (ReadLength == 0) {

            if (Collector == nullptr) {
                BufferMemoryBlock::Build(TempBlock, BUFFER_MEMORY_BLOCK_SIZE);
            } else {
                TempBlock = Collector->Get();
            }

            if (TempBlock == nullptr) {
                return {ENOMEM, "Can not allocate BufferMemoryBlock when recv()"};
            }

            WriteBlock->SetNextBlock(TempBlock);
            WriteBlock = TempBlock;
            PointerToData = Cursor.Bound = WriteBlock->Start;
            ReadLength = WriteBlock->End - PointerToData;
        }

        RecievedSize = recv(SocketFD, PointerToData, ReadLength, 0);

        if (RecievedSize == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                return {errno, "failed to read from socket!"};
            }
        } else if (RecievedSize > 0) {
            Cursor.Bound += RecievedSize;
        } else {
            break;
        }
    }
    return {0};
}

RuntimeError Buffer::ReadBytes(u_char *PointerToData, size_t DataLength) {

    size_t CurrentBlockFreeSize;
    BufferMemoryBlock *TempBufferBlock, *WriteBlock;

    if (HeadBlock == nullptr) {

        if (Collector == nullptr) {
            BufferMemoryBlock::Build(HeadBlock, BUFFER_MEMORY_BLOCK_SIZE);
        } else {
            HeadBlock = Collector->Get();
        }

        if (HeadBlock == nullptr) {
            return {ENOMEM, "can not allocate buffer block"};
        } else {
            Cursor = {this, HeadBlock->Start, HeadBlock->Start};
        }
    }

    WriteBlock = AddressToMemoryBlock(Cursor.Bound);

    for (;;) {

        CurrentBlockFreeSize = WriteBlock->End - Cursor.Bound;

        if (DataLength > CurrentBlockFreeSize) {

            if (Collector == nullptr) {
                BufferMemoryBlock::Build(TempBufferBlock, BUFFER_MEMORY_BLOCK_SIZE);
            } else {
                TempBufferBlock = Collector->Get();
            }

            if (TempBufferBlock == nullptr) {
                return {ENOMEM, "no enough memory"};
            }

            memcpy(Cursor.Bound, PointerToData, CurrentBlockFreeSize);

            PointerToData += CurrentBlockFreeSize;
            DataLength -= CurrentBlockFreeSize;

            WriteBlock->SetNextBlock(TempBufferBlock);
            Cursor.Bound = WriteBlock->Start;
        } else {
            memcpy(Cursor.Bound, PointerToData, DataLength);
            Cursor.Bound += DataLength;
            break;
        }
    }

    return {0};
}

void Buffer::Reset() {

    Cursor = {this, nullptr, nullptr};

    GC();
}

void Buffer::GC() {

    BufferMemoryBlock *TempBlock, *ReadBlock;

    ReadBlock = AddressToMemoryBlock(Cursor.Position);

    while (HeadBlock != nullptr && HeadBlock != ReadBlock) {

        TempBlock = HeadBlock -> GetNextBlock();

        if (Collector == nullptr){
            HeadBlock->Reset();
            BufferMemoryBlock::Destroy(HeadBlock);
        } else {
            Collector->Put(HeadBlock);
        }

        HeadBlock = TempBlock;
    }
}
