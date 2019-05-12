#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

MemoryBuffer::MemoryBuffer(Byte *BufferStart, Byte *BufferEnd) :
        BufferStart(BufferStart),
        BufferEnd(BufferEnd) {}

MemoryBuffer::~MemoryBuffer() {

    if (BufferStart != nullptr) {
        free(BufferStart);
    }

    BufferStart = BufferEnd = nullptr;
}

WritableMemoryBuffer::WritableMemoryBuffer(Byte *BufferStart, Byte *BufferEnd, bool Aligned) :
        MemoryBuffer(BufferStart, BufferEnd),
        Aligned(Aligned) {
}

static auto defaultAllocate = [](size_t Size, bool Aligned, Byte *& BufferStart, Byte *&BufferEnd) {

    if (Aligned) {
        BufferStart = nullptr;
        posix_memalign((void **) &BufferStart, 8, Size);
    } else {
        BufferStart = static_cast<Byte *>(malloc(Size));
    }

    if (BufferStart == nullptr) {
        BufferEnd = nullptr;
    }

    BufferEnd = BufferStart + Size;
};

size_t WritableMemoryBuffer::resize(size_t NewSize) {

    Byte *NewStart, *NewEnd;

    if (NewSize <= 0 || NewSize == getBufferSize()) {
        return getBufferSize();
    }

    defaultAllocate(NewSize, Aligned, NewStart, NewEnd);

    if (NewStart == nullptr) {
        return getBufferSize();
    }

    if (BufferStart != nullptr) {
        size_t CopySize;

        CopySize = getBufferSize();

        if (CopySize > NewSize) {
            CopySize = NewSize;
        }

        memccpy(NewStart, BufferStart,CopySize, sizeof(Byte));
        free(BufferStart);
    }

    BufferStart = NewStart, BufferEnd = NewEnd;

    return NewSize;
}

std::unique_ptr<WritableMemoryBuffer> WritableMemoryBuffer::NewBuffer(size_t Size, bool Aligned) {

    Byte *BufferStart, *BufferEnd;

    if (Size <= 0) {
        return std::unique_ptr<WritableMemoryBuffer>(nullptr);
    }

    defaultAllocate(Size, Aligned, BufferStart, BufferEnd);

    WritableMemoryBuffer *Ret = new WritableMemoryBuffer(BufferStart, BufferEnd);

    Ret->Aligned = Aligned;

    return std::unique_ptr<WritableMemoryBuffer>(Ret);
}

BufferWriter::BufferWriter(WritableMemoryBuffer &Target) :
        Buffer(Target),
        MaximumBufferSize(-1),
        Cursor(0){
}

BufferWriter::BufferWriter(WritableMemoryBuffer &Target,size_t MaximumBufferSize,size_t Offset):
        Buffer(Target),
        MaximumBufferSize(MaximumBufferSize),
        Cursor(Offset){
}

size_t BufferWriter::CheckBufferSize(size_t WriteSize) {

    // [TODO]: hacker here to add non-linear memory size selector

    size_t NewSize;

    if (Cursor + WriteSize >= Buffer.getBufferSize()) {
        // memory buffer full
        NewSize = Buffer.getBufferSize();

        // figure out new memory buffer size
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
    return Buffer.getBufferSize();
}

int BufferWriter::writeByte(const Byte B) {
    char Character = static_cast<char>(B);
    return writeString(&Character, 1);
}

int BufferWriter::writeString(const char *Str, size_t Size) {

    Byte *pByte;
    char *pChar;

    if (Cursor + Size >= CheckBufferSize(Size)) {
        return 0;
    }

    pByte = Buffer.getBufferStart();
    pByte += Cursor;

    pChar = reinterpret_cast<char *>(pByte);

    strncpy(pChar, Str, Size);

    Cursor += Size;

    return Size;
}


