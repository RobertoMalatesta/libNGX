namespace ngx::Core {

    class BufferAllocator : public Resetable{
        SpinLock Lock;
        size_t BlockSize = 0;
        BufferMemoryBlockRecycler *Recycler = nullptr;
        BufferCursor ReadCursor, WriteCursor;
        BufferMemoryBlock *HeadBlock = nullptr, *CurrentBlock = nullptr;
        friend class BufferBuilder;
        BufferAllocator(BufferMemoryBlockRecycler *Recycler, size_t BlockSize = BUFFER_DEFAULT_BLOCK_SIZE);
        ~BufferAllocator();

        BufferRange AllocateBufferRange(size_t Size);

        void GC();
        void Reset();
    };
}