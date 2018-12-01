#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

BufferBuilder::BufferBuilder(u_char Lg2BlockSize, uint64_t RecycleSize) :
        Lg2BlockSize(Lg2BlockSize),
        RecycleBin(1ULL << Lg2BlockSize, RecycleSize) {

}

BufferBuilder::~BufferBuilder() {
    RecycleBin.~BufferMemoryBlockRecycleBin();
}

bool BufferBuilder::BuildBuffer(Buffer &Buf) {

    Buf.RecycleBin = &RecycleBin;
    Buf.Lg2BlockSize = Lg2BlockSize;
    return true;
}
