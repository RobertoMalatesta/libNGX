namespace ngx::Core {

    class BufferBuilder {
    protected:
        size_t BlockSize;
        BufferMemoryBlockRecycler Recycler;
    public:
        BufferBuilder(size_t BlockSize, uint64_t RecycleSize);

        bool BuildBuffer(Buffer &Buf);

        BufferAllocator BuildBufferAllocator();
    };
}