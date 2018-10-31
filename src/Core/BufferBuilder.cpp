#include "Core/Core.h"

using namespace ngx::Core;

BufferBuilder::BufferBuilder(size_t BlockSize, uint64_t RecycleSize) :
        BlockSize(BlockSize),
        Recycler(BlockSize, RecycleSize) {}

bool BufferBuilder::BuildBuffer(Buffer &Buf) {

    Buf.Recycler = &Recycler;
    Buf.BlockSize = BlockSize;
    Buf.HeadBlock = Recycler.Get();

    if (Buf.HeadBlock == nullptr) {
        return false;
    }
    Buf.WriteCursor.Block = Buf.HeadBlock;
    Buf.WriteCursor.Position = Buf.HeadBlock->Start;
    Buf.ReadCursor << Buf.WriteCursor >> Buf.WriteCursor;
    return true;
}
