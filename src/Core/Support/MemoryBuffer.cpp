#include "Core/Support/MemoryBuffer.h"
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
using namespace std;
using namespace ngx::Core::Support;
MemoryBuffer::MemoryBuffer() : BufferStart(nullptr), BufferEnd(nullptr){}
MemoryBuffer::MemoryBuffer(Byte *BufferStart, Byte *BufferEnd) :
    BufferStart(BufferStart),
    BufferEnd(BufferEnd) {}
MemoryBuffer::~MemoryBuffer() {
    if (BufferStart != nullptr) free(BufferStart);
    BufferStart = BufferEnd = nullptr;
}
WritableMemoryBuffer::WritableMemoryBuffer(bool Aligned) :
        MemoryBuffer(nullptr, nullptr),
        Aligned(Aligned) {}
WritableMemoryBuffer::WritableMemoryBuffer(Byte *BufferStart, Byte *BufferEnd, bool Aligned) :
        MemoryBuffer(BufferStart, BufferEnd),
        Aligned(Aligned) {}
static auto defaultAllocate = [](size_t Size, bool Aligned,
        Byte *&BufferStart, Byte *&BufferEnd) {
    if (Aligned) {
        BufferStart = nullptr;
        posix_memalign((void **) &BufferStart, 8, Size);
    } else
        BufferStart = static_cast<Byte *>(malloc(Size));
    if (BufferStart == nullptr)
        BufferEnd = nullptr;
    BufferEnd = BufferStart + Size;
};
size_t WritableMemoryBuffer::resize(size_t NewSize) {
    Byte *NewStart, *NewEnd;
    if (NewSize <= 0 || NewSize == size())
        return size();
    defaultAllocate(NewSize, Aligned, NewStart, NewEnd);
    if (NewStart == nullptr)
        return size();
    if (BufferStart != nullptr) {
        size_t CopySize;
        CopySize = size();
        if (CopySize > NewSize)
            CopySize = NewSize;
        memccpy(NewStart, BufferStart, sizeof(Byte), CopySize);
        free(BufferStart);
    }
    BufferStart = NewStart, BufferEnd = NewEnd;
    return NewSize;
}

std::unique_ptr<WritableMemoryBuffer> WritableMemoryBuffer::NewBuffer(size_t Size, bool Aligned) {
    Byte *BufferStart, *BufferEnd;
    if (Size <= 0)
        return std::unique_ptr<WritableMemoryBuffer>(nullptr);
    defaultAllocate(Size, Aligned, BufferStart, BufferEnd);
    WritableMemoryBuffer *Ret = new WritableMemoryBuffer(BufferStart, BufferEnd);
    Ret->Aligned = Aligned;
    return std::unique_ptr<WritableMemoryBuffer>(Ret);
}
BufferWriter::BufferWriter(WritableMemoryBuffer &Target) :
        Buffer(Target),
        MaximumBufferSize(-1),
        Cursor(0) {}
BufferWriter::BufferWriter(WritableMemoryBuffer &Target, size_t MaximumBufferSize, size_t Offset) :
        Buffer(Target),
        MaximumBufferSize(MaximumBufferSize),
        Cursor(Offset) {}
size_t BufferWriter::CheckBufferSize(size_t WriteSize) {
    // [TODO]: hack here to add non-linear memory size selector
    size_t NewSize;
    if (Cursor + WriteSize >= Buffer.size()) {
        // memory buffer full, create an bigger one
        NewSize = Buffer.size();
        // figure out new memory buffer size
        if (NewSize == 0) NewSize = 128;
        if (MaximumBufferSize <= 0) {   // unlimited buffer size
            NewSize *= 2;
        } else if (NewSize < MaximumBufferSize) {   // smaller than maximum buffer size
            NewSize *= 2;
            if (NewSize > MaximumBufferSize) {
                NewSize = MaximumBufferSize;
            }
        }
        // resize when NewSize is different from origin size
        Buffer.resize(NewSize);
    }
    // return new buffer size
    return Buffer.size();
}
int BufferWriter::fromByte(const Byte B) {
    char Character = static_cast<char>(B);
    return fromString(&Character, 1);
}
int BufferWriter::fromString(const char *Str, size_t Size) {
    Byte *pByte;
    char *pChar;
    if (Cursor+Size+1 >= CheckBufferSize(Size)) return 0;
    pByte = Buffer.begin();
    pByte += Cursor;
    pChar = reinterpret_cast<char *>(pByte);
    strncpy(pChar, Str, Size+1);
    Cursor += Size;
    return Size;
}
int BufferWriter::fromFile(int FD, int64_t Offset, size_t Size) { return -1; }
int BufferWriter::fromSocket(int FD, uint64_t MaximumSize) {
    Byte *pByte;
    char *pChar;
    size_t ReadSize;
    ssize_t RecvSize;
    ReadSize = MaximumSize;
    if (Cursor + MaximumSize > CheckBufferSize(MaximumSize)) {
        ReadSize = Buffer.size() - Cursor;
    }
    pByte = Buffer.begin();
    pByte += Cursor;
    pChar = reinterpret_cast<char *>(pByte);
    RecvSize = recv(FD, pChar, ReadSize, 0);
    if (ReadSize == -1) return ((errno==EAGAIN)||(errno==EWOULDBLOCK))? 0 : -1 ;
    else if (ReadSize > 0)
        Cursor += RecvSize;
    return -1;
}
int BufferWriter::toFile(int FD, int64_t Offset, size_t Size) {
    Byte *pByte;
    char *pChar;
    pByte = Buffer.begin();
    pChar = reinterpret_cast<char *>(pByte);
    write(FD, pChar, Cursor);
    return -1;
}
int BufferWriter::toSocket(int FD, int64_t Offset, size_t Size) {
    return -1;
}
