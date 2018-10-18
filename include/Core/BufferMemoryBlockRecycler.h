namespace ngx::Core {

    class BufferMemoryBlockRecycler : public Recycler {
        protected:
            SpinLock Lock;
            size_t  BufferMemoryBlockSize;
        public:
            BufferMemoryBlockRecycler(size_t  BufferMemoryBlockSize, uint64_t RecyclerSize);
            size_t  GetBlockSize() { return this->BufferMemoryBlockSize; }
            virtual BufferMemoryBlock *Get();
            virtual void Put(BufferMemoryBlock *Item);
    };
}