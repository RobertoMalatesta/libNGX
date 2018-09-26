namespace ngx::Core {
    class MemoryBlock {
        protected:
            std::atomic_uint UseCount = {0};
            size_t TotalSize = 0;
            size_t FreeSize = 0;
            void *PointerToData = nullptr, *PointerToHead = nullptr;
            void * Magic = nullptr;
            MemoryBlock *Next;
        public:
            MemoryBlock(size_t Size);
            ~MemoryBlock();

            static MemoryBlock *CreateMemoryBlock(size_t Size);
            static void FreeMemoryBlock(MemoryBlock **PointerToBlock);

            static MemoryBlock *AddressToMemoryBlock(void *Address, size_t Size);

            bool IsInBlock(void *Address);
            bool IsFreeBlock() { return UseCount == 0; }
            inline void Reset() {
                PointerToData = PointerToHead;
                FreeSize = TotalSize;
            }
            void SetNext(MemoryBlock *Next) { this->Next = Next; }
            MemoryBlock *GetNext() { return  Next; }
    };

    class MemoryBlockAllocator : public MemoryBlock, MemAllocator {
        public:
            MemoryBlockAllocator(size_t Size);
            ~MemoryBlockAllocator();

            virtual void *Allocate(size_t Size);
            virtual void Free(void **Pointer);
            virtual void GC() { /*Empty Code Block*/ }

            static MemoryBlockAllocator *CreateMemoryBlockAllocator(size_t Size);
            static void FreeMemoryBlockAllocator(MemoryBlockAllocator **PointerToAllocator);

            void SetNext(MemoryBlockAllocator *Next) { this->Next = Next; }
            MemoryBlockAllocator *GetNext() { return  (MemoryBlockAllocator *)Next; }
    };

    class BufferMemoryBlock : MemoryBlock {
        protected:
            u_char *Start, *End, *Pos;
            friend class Buffer;
        public:
            BufferMemoryBlock(size_t Size);
            ~BufferMemoryBlock();

            bool IsEmpty() { return Pos >= End || Pos < Start; }

            static BufferMemoryBlock *CreateBufferMemoryBlock(size_t Size);
            static void FreeBufferMemoryBlock(BufferMemoryBlock **PointerToBlock);

            void SetNext(BufferMemoryBlock *Next) { this->Next = Next; }
            BufferMemoryBlock *GetNext() { return  (BufferMemoryBlock *)Next; }
    };
}


