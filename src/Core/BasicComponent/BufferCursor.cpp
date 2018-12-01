#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

Cursor::Cursor(u_char Lg2BlockSize, u_char *Position) : Lg2BlockSize(Lg2BlockSize), Position(Position) {
}

uint32_t Cursor::IncRef() {

    BufferMemoryBlock *MemoryBlock;
    MemoryBlock = AddressToBlock(Position, Lg2BlockSize);

    if (MemoryBlock != nullptr) {
        return MemoryBlock->IncRef();
    }

    return 0;
}

uint32_t Cursor::DecRef() {
    BufferMemoryBlock *MemoryBlock;

    MemoryBlock = AddressToBlock(Position,Lg2BlockSize);

    if (MemoryBlock != nullptr) {
        return MemoryBlock->DecRef();
    }

    return 0;
}

BufferMemoryBlock* Cursor::AddressToBlock(u_char *Position, u_char Lg2BlockSize) {

    BasicMemoryBlock *BasicBlock;

    BasicBlock = BasicMemoryBlock::AddressToMemoryBlock(Position, (1ULL << Lg2BlockSize));

    return (BufferMemoryBlock *)BasicBlock;
}

BoundCursor::BoundCursor(u_char Lg2BlockSize, u_char *Position, u_char *Bound) : Cursor(Lg2BlockSize, Position),
                                                                                  Bound(Bound) {
}

uint32_t BoundCursor::IncRef() {

    BufferMemoryBlock *LeftBlock, *RightBlock;

    LeftBlock = AddressToBlock(Position, Lg2BlockSize);
    RightBlock = AddressToBlock(Position, Lg2BlockSize);

    while (LeftBlock != RightBlock && LeftBlock != nullptr) {
        LeftBlock->IncRef();
        LeftBlock = LeftBlock->GetNextBlock();
    }

    RightBlock->IncRef();

    return 0;
}

uint32_t BoundCursor::DecRef() {

    BufferMemoryBlock *LeftBlock, *RightBlock;

    LeftBlock = AddressToBlock(Position, Lg2BlockSize);
    RightBlock = AddressToBlock(Position, Lg2BlockSize);

    while (LeftBlock != RightBlock && LeftBlock != nullptr) {
        LeftBlock->DecRef();
        LeftBlock = LeftBlock->GetNextBlock();
    }

    RightBlock->DecRef();

    return 0;
}

BoundCursor BoundCursor::operator+(size_t Size) const {

    BoundCursor R = *this;

    if (Lg2BlockSize >= LG2_MEMORY_BLOCK_SIZE_MIN && Lg2BlockSize < LG2_MEMORY_BLOCK_SIZE_MAX) {
        BufferMemoryBlock *ThisBlock, *BoundBlock;

        ThisBlock = AddressToBlock(Position, Lg2BlockSize);
        BoundBlock = AddressToBlock(Bound, Lg2BlockSize);

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

bool BoundCursor::operator!() {

    BufferMemoryBlock *L, *R;

    if (Lg2BlockSize >= LG2_MEMORY_BLOCK_SIZE_MIN && Lg2BlockSize < LG2_MEMORY_BLOCK_SIZE_MAX) {
        return true;
    }
    L = AddressToBlock(Position, Lg2BlockSize);
    R = AddressToBlock(Bound, Lg2BlockSize);

    return Position == nullptr || (L == R && Position >= Bound);
}

bool BoundCursor::ReadByte(uint32_t Offset, u_char &C1) const {

    BoundCursor TempCursor = (*this + Offset);

    if (TempCursor.Position == nullptr) {
        return false;
    }

    C1 = *TempCursor.Position;
    return true;
}

bool BoundCursor::CmpByte(uint32_t Offset, u_char C1) const {
    u_char A1;
    return this->ReadByte(Offset, A1) && A1 == C1;
}

bool BoundCursor::ReadBytes2(uint32_t Offset, u_char &C1, u_char &C2) const {

    BoundCursor Cur;
    BufferMemoryBlock *Block1, *Block2;

    Cur = (*this + (Offset + 1));

    Block1 = AddressToBlock(Position, Lg2BlockSize);
    Block2 = AddressToBlock(Cur.Position, Cur.Lg2BlockSize);

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

bool BoundCursor::CmpByte2(uint32_t Offset, u_char C1, u_char C2) const {

    u_char A1, A2;

    return this->ReadBytes2(Offset, A1, A2)
           && A1 == C1
           && A2 == C2;
}

bool BoundCursor::ReadBytes4(uint32_t Offset, u_char &C1, u_char &C2, u_char &C3, u_char &C4) const {

    BoundCursor Cur = (*this + (Offset + 3));
    BufferMemoryBlock *Block1, *Block2;

    Block1 = AddressToBlock(Position, Lg2BlockSize);
    Block2 = AddressToBlock(Cur.Position, Cur.Lg2BlockSize);

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

bool BoundCursor::CmpByte4(uint32_t Offset, u_char C1, u_char C2, u_char C3, u_char C4) const {

    u_char A1, A2, A3, A4;

    return this->ReadBytes4(Offset, A1, A2, A3, A4)
           && A1 == C1
           && A2 == C2
           && A3 == C3
           && A4 == C4;
}

size_t BoundCursor::Size() {

    size_t Size;
    BufferMemoryBlock *L, *R;


    if (!*this) {
        return 0;
    }

    L = AddressToBlock(Position, Lg2BlockSize);
    R = AddressToBlock(Bound, Lg2BlockSize);

    if (L == R) {
        Size = Bound - Position;
    } else {
        Size = L->End - Position;

        L = L->GetNextBlock();

        while (L != R && L != nullptr) {
            L = L->GetNextBlock();
            Size += L->End - L->Start;
        }

        if (L == nullptr) {
            return 0;
        }

        Size += Bound - L->Start;
    }

    return Size;
}
