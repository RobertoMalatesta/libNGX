class MemoryBlockAllocator
        : public BasicMemoryBlock,
          public Allocator,
          public CanReset,
          public CanGC,
          public Reference,
          public AlignBuild<MemoryBlockAllocator> {
protected:
    u_char *Pos;
    size_t FreeSize;
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
};

