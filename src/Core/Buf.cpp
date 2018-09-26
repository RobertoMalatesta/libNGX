#include "Core/Core.h"

using namespace ngx::Core;


Buffer::Buffer(size_t BlockSize) {

    HeadBlock = MemoryBlock::CreateMemoryBlock(BlockSize);

    if (HeadBlock == nullptr) {
        return;
    }
    this->BlockSize = BlockSize;
    CurrentBlock = HeadBlock;
}

RuntimeError Buffer::WriteDataToBuffer(u_char *PointerToData, size_t DataLength) {
    return RuntimeError(0);
}

bool Buffer::HasByte() {
    return false;
}

char* Buffer::ReadByte() {
    return nullptr;
}

void Buffer::Reset() {

}
