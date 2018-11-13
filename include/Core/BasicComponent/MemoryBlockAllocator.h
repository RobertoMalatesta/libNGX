class MemoryBlockAllocator
        : public BasicMemoryBlock,
          public MemAllocator,
          public CanReset,
          public AlignBuild<MemoryBlockAllocator, PAGE_SIZE, POOL_DEFAULT_BLOCK_SIZE> {
protected:
    MemoryBlockAllocator *Next;
public:
    MemoryBlockAllocator(size_t Size);

    ~MemoryBlockAllocator();

    virtual void *Allocate(size_t Size);

    virtual void Free(void *&Pointer);

    virtual void GC() { /*Empty Code Block*/ }

    void SetNextBlock(MemoryBlockAllocator *Next) { this->Next = Next; }

    MemoryBlockAllocator *GetNextBlock() { return Next; }

    inline void Reset() {
        PointerToData = PointerToHead, FreeSize = TotalSize;
    };
};

