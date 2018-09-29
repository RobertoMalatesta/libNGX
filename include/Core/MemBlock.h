namespace ngx::Core {
    class MemoryBlock {
        protected:
            std::atomic_uint UseCount = {0};
            size_t TotalSize = 0;
            size_t FreeSize = 0;
            void *PointerToData = nullptr, *PointerToHead = nullptr;
            void * Magic = nullptr;

        public:
            MemoryBlock(size_t Size);
            ~MemoryBlock();

            static MemoryBlock *Build(size_t Size);
            static void Destroy(MemoryBlock **PointerToBlock);

            static MemoryBlock *AddressToMemoryBlock(void *Address, size_t Size);

            bool IsInBlock(void *Address);
            bool IsFreeBlock() { return UseCount == 0; }
            inline void Reset() {
                PointerToData = PointerToHead;
                FreeSize = TotalSize;
            }
    };

    class MemoryBlockAllocator : public MemoryBlock, public MemAllocator {
        protected:
            MemoryBlockAllocator *Next;
        public:
            MemoryBlockAllocator(size_t Size);
            ~MemoryBlockAllocator();

            virtual void *Allocate(size_t Size);
            virtual void Free(void **Pointer);
            virtual void GC() { /*Empty Code Block*/ }

            static MemoryBlockAllocator *Build(size_t Size);
            static void Destroy(MemoryBlockAllocator **PointerToAllocator);

            void SetNextBlock(MemoryBlockAllocator *Next) { this->Next = Next; }
            MemoryBlockAllocator *GetNextBlock() { return Next; }
    };

    class BufferMemoryBlock : public MemoryBlock, public Recyclable {
        protected:
            u_char *Start, *End, *Pos;
            BufferMemoryBlock *NextBlock;
            friend class Buffer;
        public:
            BufferMemoryBlock(size_t Size);
            ~BufferMemoryBlock();

            inline void Reset() { Pos = Start; }

            static BufferMemoryBlock *Build(size_t Size);
            static void Destroy(BufferMemoryBlock **PointerToBlock);

            void SetNextBlock(BufferMemoryBlock *Next) { this->NextBlock = Next; }
            BufferMemoryBlock *GetNextBlock() { return NextBlock; }
    };
}


