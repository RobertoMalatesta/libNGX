namespace ngx::Core {

    class BufferBuilder {
        protected:
            size_t BlockSize;
            BufferMemoryBlockRecycler Recycler;
        public:
            BufferBuilder(size_t BlockSize, int RecycleSize) : Recycler(BlockSize, RecycleSize), BlockSize(BlockSize) {}
            Buffer BuildBuffer();
            BufferAllocator BuildBufferAllocator();
    };
}