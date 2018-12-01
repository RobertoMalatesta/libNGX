#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

BufferBuilder::BufferBuilder(size_t BlockSize, uint64_t RecycleSize) :
        BlockSize(BlockSize),
        RecycleBin(BlockSize, RecycleSize) {

}

BufferBuilder::~BufferBuilder() {
    RecycleBin.~BufferMemoryBlockRecycleBin();
}

bool BufferBuilder::BuildBuffer(Buffer &Buf) {

    Buf.RecycleBin = &RecycleBin;
    Buf.BlockSize = BlockSize;
    return true;
}
