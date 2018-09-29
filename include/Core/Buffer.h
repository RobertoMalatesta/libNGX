namespace ngx::Core {

    class Buffer : public Resetable{
        protected:
            SpinLock Lock;
            size_t BlockSize = 0;
            BufferMemoryBlockRecycler *Recycler = nullptr;
            BufferMemoryBlock *HeadBlock = nullptr, *CurrentBlock = nullptr;
            BufferMemoryBlock *ReadBlock = nullptr, *WriteBlock = nullptr;
            u_char *ReadPosition= nullptr, *WritePosition = nullptr;
        public:
            Buffer(BufferMemoryBlockRecycler *Recycler, size_t BlockSize = BUFFER_DEFAULT_BLOCK_SIZE);
            RuntimeError WriteDataToBuffer(u_char *PointerToData, size_t DataLength);
            u_char ReadByte();
            void Reset();
    };

    class BufferMemoryBlockRecycler : public Recycler {
        protected:
            SpinLock Lock;
            size_t  BufferMemoryBlockSize;
        public:
            BufferMemoryBlockRecycler(size_t  BufferMemoryBlockSize, uint64_t RecyclerSize, MemAllocator *Allocator);
            size_t  GetBlockSize() { return this->BufferMemoryBlockSize; }
            BufferMemoryBlock *Get();
            void Put(BufferMemoryBlock *Item);
    };

}