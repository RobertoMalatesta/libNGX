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

    if (HeadBlock == nullptr) {

        if (Collector == nullptr) {
            BufferMemoryBlock::Build(HeadBlock, BUFFER_MEMORY_BLOCK_SIZE);
        } else {
            HeadBlock = Collector->Get();
        }

        if (HeadBlock == nullptr) {
            return {ENOMEM, "can not allocate buffer block"};
        } else {
            Cursor = {this,  HeadBlock->Start, HeadBlock->Start};
        }
    }

    BufferMemoryBlock *WriteBlock;

    WriteBlock = AddressToMemoryBlock(Cursor.Bound);

    while (true) {

        size_t FreeSize = WriteBlock->End - Cursor.Bound;

        if (FreeSize == 0) {

            BufferMemoryBlock *NewBlock = nullptr;

            if (Collector == nullptr) {
                BufferMemoryBlock::Build(NewBlock, BUFFER_MEMORY_BLOCK_SIZE);
            } else {
                NewBlock = Collector->Get();
            }

            if (NewBlock == nullptr) {
                return {ENOMEM, "Can not allocate BufferMemoryBlock when recv()"};
            }

            WriteBlock->SetNextBlock(NewBlock);
            WriteBlock = WriteBlock->GetNextBlock();

            Cursor.Bound = WriteBlock->Start;

            FreeSize = WriteBlock->End - Cursor.Bound;
        }

        int SocketFD = C->GetSocketFD();

        ssize_t ReadSize = recv(SocketFD, Cursor.Bound, FreeSize, 0);

        if (ReadSize == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                return {errno, "failed to read from socket!"};
            }
        } else if (ReadSize > 0) {
            Cursor.Bound += ReadSize;
        } else {
            break;
        }
    }
    return {0};
}

RuntimeError Buffer::ReadBytes(u_char *PointerToData, size_t DataLength) {

    size_t FreeSize;
    BufferMemoryBlock *WriteBlock;

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

        FreeSize = WriteBlock->End - Cursor.Bound;

        if (DataLength > FreeSize) {

            BufferMemoryBlock *NewBlock = nullptr;

            if (Collector == nullptr) {
                BufferMemoryBlock::Build(NewBlock, BUFFER_MEMORY_BLOCK_SIZE);
            } else {
                NewBlock = Collector->Get();
            }

            if (NewBlock == nullptr) {
                return {ENOMEM, "no enough memory"};
            }

            memcpy(Cursor.Bound, PointerToData, FreeSize);

            PointerToData += FreeSize, DataLength -= FreeSize;

            WriteBlock->SetNextBlock(NewBlock);
            WriteBlock = WriteBlock->GetNextBlock();

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

    Cursor = {this, nullptr, nullptr}, GC();
}

void Buffer::GC() {

    BufferMemoryBlock *NextBlock, *ReadBlock;

    ReadBlock = AddressToMemoryBlock(Cursor.Position);

    while (HeadBlock != nullptr && HeadBlock != ReadBlock) {

        NextBlock = HeadBlock->GetNextBlock();

        if (Collector == nullptr) {
            HeadBlock->Reset();
            BufferMemoryBlock::Destroy(HeadBlock);
        } else {
            Collector->Put(HeadBlock);
        }

        HeadBlock = NextBlock;
    }
}
