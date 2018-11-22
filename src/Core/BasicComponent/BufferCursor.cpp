#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

Cursor::Cursor(Buffer *ParentBuffer, u_char *Position) : ParentBuffer(ParentBuffer), Position(Position) {
    this->ParentBuffer = ParentBuffer, this->Position = Position;
}

BoundCursor BoundCursor::operator+(size_t Size) const {

    BoundCursor R = *this;

    if (ParentBuffer != nullptr) {
        BufferMemoryBlock *ThisBlock, *BoundBlock;

        ThisBlock = ParentBuffer->AddressToMemoryBlock(Position);
        BoundBlock = ParentBuffer->AddressToMemoryBlock(Bound);

        if (ThisBlock != nullptr && BoundBlock != nullptr) {
            while (Size > 0) {

                if ((R.Position + Size) < ThisBlock->End) {
                    if (ThisBlock == BoundBlock && (R.Position + Size) >= Bound) {
                        R.Position = nullptr;
                        break;
                    } else {
                        R.Position += Size, Size = 0;
                    }
                } else {
                    Size -= (ThisBlock->End - R.Position);
                    ThisBlock = ThisBlock->GetNextBlock();
                    R.Position = ThisBlock->Start;
                }
            }
        }
    } else {
        R.Position = R.Bound = nullptr;
    }
    return R;
}

bool BoundCursor::ReadBytes2(uint32_t Offset, u_char &C1, u_char &C2) const {

    BoundCursor Cur;
    BufferMemoryBlock *Block1, *Block2;

    Cur = (*this + (Offset + 1));

    Block1 = ParentBuffer->AddressToMemoryBlock(Position);
    Block2 = ParentBuffer->AddressToMemoryBlock(Cur.Position);

    if (Cur.Position == nullptr || Block1 == nullptr || Block2 == nullptr) {
        return false;
    } else {

        C2 = *(Cur.Position);

        if (Block1 == Block2) {
            C1 = *(Cur.Position - 1);
        } else {
            return this->ReadByte(Offset, C1);
        }
        return true;
    }
}

bool BoundCursor::ReadBytes4(uint32_t Offset, u_char &C1, u_char &C2, u_char &C3, u_char &C4) const {

    BoundCursor Cur = (*this + (Offset + 3));
    BufferMemoryBlock *Block1, *Block2;

    Block1 = ParentBuffer->AddressToMemoryBlock(Position);
    Block2 = ParentBuffer->AddressToMemoryBlock(Cur.Position);

    if (Cur.Position == nullptr || Block1 == nullptr || Block2 == nullptr) {
        return false;
    } else {

        C4 = *(Cur.Position);

        if (Block1 == Block2) {
            C1 = *(Cur.Position - 3);
            C2 = *(Cur.Position - 2);
            C3 = *(Cur.Position - 1);
        } else {
            return this->ReadBytes2(Offset, C1, C2) && this->ReadByte(Offset + 2, C3);
        }
    }
    return true;
}

uint32_t Cursor::IncRef() {

    BufferMemoryBlock *MemoryBlock;

    MemoryBlock = ParentBuffer->AddressToMemoryBlock(Position);

    if (MemoryBlock != nullptr) {
        return MemoryBlock->IncRef();
    }

    return 0;
}

uint32_t Cursor::DecRef() {
    BufferMemoryBlock *MemoryBlock;

    MemoryBlock = ParentBuffer->AddressToMemoryBlock(Position);

    if (MemoryBlock != nullptr) {
        return MemoryBlock->DecRef();
    }

    return 0;
}

BoundCursor::BoundCursor(Buffer *ParentBuffer, u_char *Position, u_char *Bound) : Cursor(ParentBuffer, Position),
                                                                                  Bound(Bound) {
}

uint32_t BoundCursor::IncRef() {

    BufferMemoryBlock *LeftBlock, *RightBlock;

    LeftBlock = ParentBuffer->AddressToMemoryBlock(Position);
    RightBlock = ParentBuffer->AddressToMemoryBlock(Bound);

    while (LeftBlock != RightBlock && LeftBlock != nullptr) {
        LeftBlock->IncRef();
        LeftBlock = LeftBlock->GetNextBlock();
    }

    RightBlock->IncRef();

    return 0;
}


uint32_t BoundCursor::DecRef() {

    BufferMemoryBlock *LeftBlock, *RightBlock;

    LeftBlock = ParentBuffer->AddressToMemoryBlock(Position);
    RightBlock = ParentBuffer->AddressToMemoryBlock(Bound);


    while (LeftBlock != RightBlock && LeftBlock != nullptr) {
        LeftBlock->DecRef();
        LeftBlock = LeftBlock->GetNextBlock();
    }

    RightBlock->DecRef();

    return 0;
}