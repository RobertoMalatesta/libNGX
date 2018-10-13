namespace ngx::Core {

    class BufferMemoryBlock : public MemoryBlock, public Recyclable {
        protected:
            BufferMemoryBlock *NextBlock;
            BufferMemoryBlock(size_t Size);
            ~BufferMemoryBlock();
            friend class Buffer;
        public:
            u_char *Start, *End, *Pos;

            static BufferMemoryBlock *Build(size_t Size);
            static void Destroy(BufferMemoryBlock **PointerToBlock);

            void SetNextBlock(BufferMemoryBlock *Next) { this->NextBlock = Next; }
            BufferMemoryBlock *GetNextBlock() { return NextBlock; }
    };

    struct BufferCursor {
        BufferMemoryBlock *Block;
        u_char *Position;
    };

    struct BufferRange {
       BufferCursor Cursor;
       size_t Length;
    };

    class Buffer : public Resetable {
        protected:
            SpinLock Lock;
            size_t BlockSize = 0;
            BufferMemoryBlockRecycler *Recycler = nullptr;
            BufferCursor ReadCursor, WriteCursor;
            BufferMemoryBlock *HeadBlock = nullptr, *CurrentBlock = nullptr;
        public:
            Buffer(BufferMemoryBlockRecycler *Recycler, size_t BlockSize = BUFFER_DEFAULT_BLOCK_SIZE);
            RuntimeError WriteDataToBuffer(u_char *PointerToData, size_t DataLength);
            u_char ReadByte();
            virtual void Reset() {};
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