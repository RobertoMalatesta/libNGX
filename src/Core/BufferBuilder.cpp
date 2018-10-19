#include "Core/Core.h"

using namespace ngx::Core;

BufferBuilder::BufferBuilder(size_t BlockSize, uint64_t RecycleSize):
BlockSize(BlockSize),
Recycler(BlockSize, RecycleSize) {}

Buffer BufferBuilder::BuildBuffer() {
    return Buffer(&Recycler, BlockSize);
}
