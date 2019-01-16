class MemoryBlockAllocator
        : public BasicMemoryBlock,
          public Allocator,
          public CanReset,
          public CanGC,
          public Reference{
protected:
    u_char *Pos;
    MemoryBlockAllocator *Next;
public:
    MemoryBlockAllocator(size_t Size);

    ~MemoryBlockAllocator() = default;

    virtual void *Allocate(size_t Size);

    virtual void Free(void *&Pointer);

    virtual void GC() { /*Empty Code Block*/ }

    /** Get start address from address of end */
    inline bool IsInBlock(void *Address) const {
        return Address >= Start && Address < End;
    }

    /** Get start address from address of end */
    virtual void Reset();

    void SetNextBlock(MemoryBlockAllocator *Next) { this->Next = Next; }

    MemoryBlockAllocator *GetNextBlock() { return Next; }

    inline bool IsFreeBlock() {
        return RefCount() == 0;
    }


    static inline MemoryBlockAllocator *Build(size_t Size) {

        int Code;
        void *TempPointer = nullptr;

        Code = posix_memalign(&TempPointer, Size, Size);

        if (Code != 0 || TempPointer == nullptr) {
            return nullptr;
        }

        return new(TempPointer) MemoryBlockAllocator(Size);
    };

    static inline int Destroy(MemoryBlockAllocator *&Item) {

        if (nullptr == Item) {
            return 0;
        }

        free((void *) Item);

        Item = nullptr;
        return 0;
    };
};

