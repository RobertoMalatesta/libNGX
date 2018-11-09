namespace ngx{
    namespace Core {
        class MemoryBlockAllocator : public MemoryBlock, public MemAllocator, public Resetable {
        protected:
            MemoryBlockAllocator *Next;

            MemoryBlockAllocator(size_t Size);

            ~MemoryBlockAllocator();

        public:

            virtual void *Allocate(size_t Size);

            virtual void Free(void **Pointer);

            virtual void GC() { /*Empty Code Block*/ }

            static MemoryBlockAllocator *Build(size_t Size);

            static void Destroy(MemoryBlockAllocator* &PointerToAllocator);

            void SetNextBlock(MemoryBlockAllocator *Next) { this->Next = Next; }

            MemoryBlockAllocator *GetNextBlock() { return Next; }

            void Reset() {
                PointerToData = PointerToHead;
                FreeSize = TotalSize;
            };
        };

    }
}