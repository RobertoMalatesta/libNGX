#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

Cursor::Cursor(Buffer *ParentBuffer, u_char *Position) : ParentBuffer(ParentBuffer), Position(Position) {
    this->ParentBuffer = ParentBuffer, this->Position = Position;
}

BufferMemoryBlock *Cursor::GetParentBlock() {

    if (ParentBuffer == nullptr) {
        return nullptr;
    }

    return ParentBuffer->AddressToMemoryBlock(Position);
}

