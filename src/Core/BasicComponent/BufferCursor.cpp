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

BoundCursor::BoundCursor(Buffer *ParentBuffer, u_char *Position, u_char *Bound) : Cursor(ParentBuffer, Position),
                                                                                  Bound(Bound) {
}

BoundCursor BoundCursor::operator+(size_t Size) const {

    BoundCursor R = *this;

    if (ParentBuffer != nullptr) {
        BufferMemoryBlock *ThisBlock, *BoundBlock;

        ThisBlock = ParentBuffer->AddressToMemoryBlock(Position);
        BoundBlock = ParentBuffer->AddressToMemoryBlock(Bound);

        if (ThisBlock != nullptr && BoundBlock != nullptr) {
            while (Size > 0) {

                if ((R.Position + Size) < ThisBlock->End()) {
                    if (ThisBlock == BoundBlock && (R.Position + Size) >= Bound) {
                        R.Position = nullptr;
                        break;
                    } else {
                        R.Position += Size, Size = 0;
                    }
                } else {
                    Size -= (ThisBlock->End() - R.Position);
                    ThisBlock = ThisBlock->GetNextBlock();
                    R.Position = ThisBlock->Start();
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

size_t BoundCursor::ReadBytes(u_char *Pointer, size_t Size) {

    size_t TempSize;
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
        TempSize = Block->End() - Block->Start();
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
        TempPointer = Block->Start();

        // decrease compare size
        Size -= TempSize;
        DestPointer += TempSize;

        // adjust compare size
        if (Block == BoundBlock) {
            TempSize = Bound - TempPointer;
        } else {
            TempSize = Block->End() - Block->Start();
        }
    }

    memcpy(DestPointer, TempPointer, Size);
    DestPointer += Size;
    return (DestPointer - Pointer);
}

bool BoundCursor::CmpBytes(u_char *Pointer, size_t Size) {

    size_t TempSize;
    u_char *TempPointer;
    BufferMemoryBlock *Block, *BoundBlock;

    TempPointer = Position;
    Block = ParentBuffer->AddressToMemoryBlock(Position);
    BoundBlock = ParentBuffer->AddressToMemoryBlock(Bound);

    // bad cursor? return true if compare with null( or empty) string
    if (ParentBuffer == nullptr
        || Block == nullptr
        || BoundBlock == nullptr) {

        return Pointer == nullptr || Size == 0;
    }

    // the remaining readable size of this block
    if (Block == BoundBlock) {
        TempSize = Bound - TempPointer;
    } else {
        TempSize = Block->End() - Block->Start();
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
        TempPointer = Block->Start();

        // decrease compare size
        Size -= TempSize;
        Pointer += TempSize;

        // adjust compare size
        if (Block == BoundBlock) {
            TempSize = Bound - TempPointer;
        } else {
            TempSize = Block->End() - Block->Start();
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
        Size = L->End() - Position;

        L = L->GetNextBlock();

        while (L != R && L != nullptr) {
            L = L->GetNextBlock();
            Size += L->End() - L->Start();
        }

        if (L == nullptr) {
            return 0;
        }

        Size += Bound - L->Start();
    }

    return Size;
}
