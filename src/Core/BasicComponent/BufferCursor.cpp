#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

Cursor::Cursor(Buffer *ParentBuffer, u_char *Position) : ParentBuffer(ParentBuffer), Position(Position) {
    this->ParentBuffer = ParentBuffer, this->Position = Position;
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

BufferMemoryBlock *Cursor::GetParentBlock() {

    if (ParentBuffer == nullptr) {
        return nullptr;
    }

    return ParentBuffer->AddressToMemoryBlock(Position);
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

bool BoundCursor::operator!() {

    BufferMemoryBlock *L, *R;

    if (ParentBuffer == nullptr || Position == nullptr) {
        return true;
    }

    L = ParentBuffer->AddressToMemoryBlock(Position);
    R = ParentBuffer->AddressToMemoryBlock(Bound);

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

bool BoundCursor::CmpByte2(uint32_t Offset, u_char C1, u_char C2) const {

    u_char A1, A2;

    return this->ReadBytes2(Offset, A1, A2)
           && A1 == C1
           && A2 == C2;
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

bool BoundCursor::CmpByte4(uint32_t Offset, u_char C1, u_char C2, u_char C3, u_char C4) const {

    u_char A1, A2, A3, A4;

    return this->ReadBytes4(Offset, A1, A2, A3, A4)
           && A1 == C1
           && A2 == C2
           && A3 == C3
           && A4 == C4;
}

size_t BoundCursor::ReadBytes(u_char *Pointer, size_t Size) {

    size_t  TempSize;
    u_char *TempPointer, *DestPointer;
    BufferMemoryBlock *Block, *BoundBlock;

    TempPointer = Position, DestPointer = Pointer;

    Block = ParentBuffer->AddressToMemoryBlock(Position);
    BoundBlock = ParentBuffer->AddressToMemoryBlock(Bound);

    // bad cursor? so we can not read
    if (ParentBuffer == nullptr
        || Block == nullptr
        || BoundBlock == nullptr) {
        return 0;
    }

    // the remaining readable size of this block
    if (Block == BoundBlock) {
        TempSize = Bound - TempPointer;
    } else {
        TempSize = Block->End - Block->Start;
    }

    // remains size exceeded this block remain size?
    while (Size > TempSize) {

        // compare data in local block
        memcpy(DestPointer, TempPointer, TempSize);

        // move to next block
        Block = Block->GetNextBlock();

        // no more data readable in buffer, just return the read byte count
        if (Block == nullptr) {
            return DestPointer - Pointer;
        }

        // move to next block start position
        TempPointer = Block->Start;

        // decrease compare size
        Size -= TempSize;
        DestPointer += TempSize;

        // adjust compare size
        if (Block == BoundBlock) {
            TempSize = Bound - TempPointer;
        } else {
            TempSize = Block->End - Block->Start;
        }
    }

    memcpy(DestPointer, TempPointer, Size);
    DestPointer += Size;
    return (DestPointer - Pointer);
}


bool BoundCursor::CmpBytes(u_char *Pointer, size_t Size) {

    size_t  TempSize;
    u_char *TempPointer;
    BufferMemoryBlock *Block, *BoundBlock;

    TempPointer = Position;
    Block = ParentBuffer->AddressToMemoryBlock(Position);
    BoundBlock = ParentBuffer->AddressToMemoryBlock(Bound);

    // bad cursor? return true if compare with null( or empty) string
    if (ParentBuffer == nullptr
        || Block == nullptr
        || BoundBlock == nullptr) {

        return  Pointer == nullptr || Size == 0;
    }

    // the remaining readable size of this block
    if (Block == BoundBlock) {
        TempSize = Bound - TempPointer;
    } else {
        TempSize = Block->End - Block->Start;
    }

    // remains size exceeded this block remain size?
    while (Size > TempSize) {

        // compare data in local block
        if (memcmp(TempPointer, Pointer, TempSize) != 0) {
            return false;
        }

        // move to next block
        Block = Block->GetNextBlock();

        // no more data readable in buffer
        if (Block == nullptr) {
            return false;
        }

        // move to next block start position
        TempPointer = Block->Start;

        // decrease compare size
        Size -= TempSize;
        Pointer += TempSize;

        // adjust compare size
        if (Block == BoundBlock) {
            TempSize = Bound - TempPointer;
        } else {
            TempSize = Block->End - Block->Start;
        }
    }

    // remaining part is in this block. just compare it
    return memcmp(TempPointer, Pointer, Size) == 0;
}

size_t BoundCursor::Size() {

    size_t Size;
    BufferMemoryBlock *L, *R;

    if (ParentBuffer == nullptr || Position == nullptr) {
        return 0;
    }

    L = ParentBuffer->AddressToMemoryBlock(Position);
    R = ParentBuffer->AddressToMemoryBlock(Bound);

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
