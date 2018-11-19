class Pool : public MemAllocator, public CanReset {
private:
    uint32_t AllocateRound = 0;
protected:
    size_t BlockSize;
    MemoryBlockAllocator *HeadBlock, *CurrentBlock;

public:
    Pool(size_t BlockSize = POOL_MEMORY_BLOCK_SIZE);

    Pool(Pool &Copy);

    ~Pool();

    virtual void *Allocate(size_t Size);

    virtual void Free(void *&pointer);

    virtual void GC();

    virtual void Reset();
};
