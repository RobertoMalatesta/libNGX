class Buffer;

struct Cursor {

    u_char *Position = nullptr;
    Buffer *ParentBuffer = nullptr;

    Cursor() = default;

    Cursor(Buffer *ParentBuffer, u_char *Position);

    uint32_t IncRef();

    uint32_t DecRef();

    inline u_char operator*() const {
        return (!*this) ? (u_char) '\0' : *Position;
    }

    inline bool operator!() const {
        return Position == nullptr;
    }

    inline Cursor &operator=(Cursor const &Right) {
        ParentBuffer = Right.ParentBuffer, Position = Right.Position;
        return *this;
    }

    inline bool operator==(Cursor const &Right) const {
        return Position == Right.Position;
    }
};

struct BoundCursor : public Cursor {

    u_char *Bound = nullptr;

    BoundCursor() = default;

    BoundCursor(Buffer *ParentBuffer, u_char *Position, u_char *Bound);

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

    inline bool operator!() const {

        return Position == nullptr || Position >= Bound;
    }

    inline u_char operator[](uint16_t Offset) const {

        return *(*this + Offset);
    }

    inline BoundCursor &operator=(BoundCursor const &Right) {

        ParentBuffer = Right.ParentBuffer, Position = Right.Position, Bound = Right.Bound;
        return *this;
    }

    inline BoundCursor &operator<(Cursor Cursor) {

        ParentBuffer = Cursor.ParentBuffer, Position = Cursor.Position;
        return *this;
    }

    inline BoundCursor &operator>(Cursor Cursor) {

        ParentBuffer = Cursor.ParentBuffer, this->Bound = Cursor.Position;
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

    bool ReadByte(uint32_t Offset, u_char &C1) const {

        BoundCursor TempCursor = (*this + Offset);

        if (TempCursor.Position == nullptr) {
            return false;
        }

        C1 = *TempCursor.Position;
        return true;
    }

    bool CmpByte(uint32_t Offset, u_char C1) const {

        u_char A1;
        return this->ReadByte(Offset, A1) && A1 == C1;
    }

    bool ReadBytes2(uint32_t Offset, u_char &C1, u_char &C2) const;

    bool CmpByte2(uint32_t Offset, u_char C1, u_char C2) const {

        u_char A1, A2;

        return this->ReadBytes2(Offset, A1, A2)
               && A1 == C1
               && A2 == C2;
    }

    bool ReadBytes4(uint32_t Offset, u_char &C1, u_char &C2, u_char &C3, u_char &C4) const;

    bool CmpByte4(uint32_t Offset, u_char C1, u_char C2, u_char C3, u_char C4) const {

        u_char A1, A2, A3, A4;

        return this->ReadBytes4(Offset, A1, A2, A3, A4)
               && A1 == C1
               && A2 == C2
               && A3 == C3
               && A4 == C4;
    }

    size_t Size() const;
};

struct HashBoundCursor : BoundCursor {
    inline uint32_t Hash() const;
};

