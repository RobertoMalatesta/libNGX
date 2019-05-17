#include "Core/Core.h"

using namespace ngx::Core::Support;

BufferBuilder::BufferBuilder(size_t BlockSize, uint32_t CollectorSize) :
        BackendCollector(BlockSize, CollectorSize) {
}

BufferBuilder::~BufferBuilder() {

    BackendCollector.Reset();
}

bool BufferBuilder::BuildBuffer(Buffer &Buf) {

    Buf.BlockSize = BackendCollector.GetBlockSize();
    Buf.Collector = &BackendCollector;
    return true;
}
