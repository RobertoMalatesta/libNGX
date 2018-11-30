class MemoryBlockAllocator
        : public BasicMemoryBlock,
          public Allocator,
          public CanReset,
          public CanGC,
          public AlignBuild<MemoryBlockAllocator> {
protected:
    MemoryBlockAllocator *Next;
public:
    MemoryBlockAllocator(size_t Size);

    ~MemoryBlockAllocator() = default;

    virtual void *Allocate(size_t Size);

    virtual void Free(void *&Pointer);

    virtual void GC() { /*Empty Code Block*/ }

    void SetNextBlock(MemoryBlockAllocator *Next) { this->Next = Next; }

    MemoryBlockAllocator *GetNextBlock() { return Next; }

    inline void Reset() {
        PointerToData = PointerToHead, FreeSize = TotalSize;
    };
};

