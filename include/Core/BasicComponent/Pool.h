class Pool : public Allocator, public CanReset, public CanGC {
private:
    uint32_t AllocateRound = 0;
protected:
    u_char Lg2BlockSize;
    MemoryBlockAllocator *HeadBlock, *CurrentBlock;

public:
    Pool(u_char Lg2BlockSize = LG2_POOL_MEMORY_BLOCK_SIZE);

    Pool(Pool &Copy);

    ~Pool();

    virtual void *Allocate(size_t Size);

    virtual void Free(void *&pointer);

    virtual void GC();

    virtual void Reset();
};
