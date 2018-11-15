#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

BufferBuilder::BufferBuilder(size_t BlockSize, uint64_t RecycleSize) :
        BlockSize(BlockSize),
        RecycleBin(BlockSize, RecycleSize) {}

bool BufferBuilder::BuildBuffer(Buffer &Buf) {

    Buf.RecycleBin = &RecycleBin;
    Buf.BlockSize = BlockSize;
    Buf.HeadBlock = RecycleBin.Get();

    if (Buf.HeadBlock == nullptr) {
        return false;
    }
    Buf.WriteCursor.Position = Buf.HeadBlock->Start;
    Buf.ReadCursor < Buf.WriteCursor > Buf.WriteCursor;
    return true;
}
