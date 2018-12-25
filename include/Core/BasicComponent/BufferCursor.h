class Buffer;

struct Cursor {

    u_char *Position = nullptr;
    Buffer *ParentBuffer = nullptr;

    Cursor() = default;

    Cursor(Buffer *ParentBuffer, u_char *Position);

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

    inline Buffer *GetParentBuffer() {
        return ParentBuffer;
    }

    BufferMemoryBlock *GetParentBlock();
};

struct BoundCursor : public Cursor {

    u_char *Bound = nullptr;

    BoundCursor() = default;

    BoundCursor(Buffer *ParentBuffer, u_char *Position, u_char *Bound);

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
        ParentBuffer = Cursor.ParentBuffer;
        Position = Cursor.Position;
        return *this;
    }

    inline BoundCursor &operator>(BoundCursor Cursor) {
        ParentBuffer = Cursor.ParentBuffer;
        this->Bound = Cursor.Position;
        return *this;
    }

    BoundCursor operator-(size_t) = delete;

    BoundCursor &operator--() = delete;

    BoundCursor &operator-=(size_t Size) = delete;

    size_t ReadBytes(u_char *Pointer, size_t Size);

    bool CmpBytes(u_char *Pointer, size_t Size);

    size_t Size();
};

