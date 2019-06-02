#include "Core/Support/Buffer.h"
#include "Core/Support/Cursor.h"

using namespace ngx::Core::Support;

Cursor::Cursor(Buffer *ParentBuffer, u_char *Position) : ParentBuffer(ParentBuffer), Position(Position) {
    this->ParentBuffer = ParentBuffer, this->Position = Position;
}

BufferMemoryBlock *Cursor::GetParentBlock() {

    if (ParentBuffer == nullptr) {
        return nullptr;
    }

    return ParentBuffer->AddressToMemoryBlock(Position);
}

