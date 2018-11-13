class Pool : public MemAllocator, public CanReset {
private:
    size_t BlockSize;
    MemoryBlockAllocator *HeadBlock, *CurrentBlock;

public:
    Pool(size_t BlockSize = PAGE_SIZE);

    Pool(Pool &Copy);

    ~Pool();

    virtual void *Allocate(size_t Size);

    virtual void Free(void *&pointer);

    virtual void GC();

    virtual void Reset();
};
