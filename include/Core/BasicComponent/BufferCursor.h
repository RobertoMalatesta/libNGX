class Buffer;

struct Cursor {
    u_char Lg2BlockSize;
    u_char *Position = nullptr;

    Cursor() = default;

    Cursor(u_char Lg2BlockSize, u_char *Position);

    uint32_t IncRef();

    uint32_t DecRef();

    inline u_char operator*() const {
        return (!*this) ? (u_char) '\0' : *Position;
    }

    inline bool operator!() const {
        return Position == nullptr;
    }

    inline Cursor &operator=(Cursor const &Right) {
        Lg2BlockSize = Right.Lg2BlockSize, Position = Right.Position;
        return *this;
    }

    inline bool operator==(Cursor const &Right) const {
        return Position == Right.Position;
    }

    static BufferMemoryBlock *AddressToBlock(u_char* Position, u_char Lg2BlockSize);
};

struct BoundCursor : public Cursor {

    u_char *Bound = nullptr;

    BoundCursor() = default;

    BoundCursor(u_char Lg2BlockSize, u_char *Position, u_char *Bound);

    uint32_t IncRef();

    uint32_t DecRef();

    BoundCursor operator+(size_t Size) const;

    inline BoundCursor operator+=(size_t Size) {

        return *this = *this + Size;
    }

    inline BoundCursor operator++() {

        return *this = *this + 1;
    }

    inline const BoundCursor operator++(int) {

        BoundCursor Ret = *this;
        *this = *this + 1;
        return Ret;
    }

    bool operator!();

    inline u_char operator[](uint16_t Offset) const {

        return *(*this + Offset);
    }

    inline BoundCursor &operator=(BoundCursor const &Right) {

        Lg2BlockSize = Right.Lg2BlockSize,  Position = Right.Position, Bound = Right.Bound;
        return *this;
    }

    inline BoundCursor &operator<(Cursor Cursor) {

        Lg2BlockSize = Cursor.Lg2BlockSize, Position = Cursor.Position;
        return *this;
    }

    inline BoundCursor &operator>(Cursor Cursor) {

        Lg2BlockSize = Cursor.Lg2BlockSize, this->Bound = Cursor.Position;
        return *this;
    }

    inline BoundCursor &operator<(BoundCursor Cursor) {

        Position = Cursor.Position;
        return *this;
    }

    inline BoundCursor &operator>(BoundCursor Cursor) {

        this->Bound = Cursor.Position;
        return *this;
    }

    BoundCursor operator-(size_t) = delete;

    BoundCursor &operator--() = delete;

    BoundCursor &operator-=(size_t Size) = delete;

    bool ReadByte(uint32_t Offset, u_char &C1) const;

    bool CmpByte(uint32_t Offset, u_char C1) const;

    bool ReadBytes2(uint32_t Offset, u_char &C1, u_char &C2) const;

    bool CmpByte2(uint32_t Offset, u_char C1, u_char C2) const;

    bool ReadBytes4(uint32_t Offset, u_char &C1, u_char &C2, u_char &C3, u_char &C4) const;

    bool CmpByte4(uint32_t Offset, u_char C1, u_char C2, u_char C3, u_char C4) const;

    size_t Size();
};

