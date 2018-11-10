namespace ngx {
    namespace Core {
        class MemoryBlockAllocator
                : public BasicMemoryBlock,
                  public MemAllocator,
                  public Resetable,
                  public AlignBuild<MemoryBlockAllocator> {
        protected:
            MemoryBlockAllocator *Next;
        public:
            MemoryBlockAllocator(size_t Size);

            ~MemoryBlockAllocator();

            virtual void *Allocate(size_t Size);

            virtual void Free(void* &Pointer);

            virtual void GC() { /*Empty Code Block*/ }

            void SetNextBlock(MemoryBlockAllocator *Next) { this->Next = Next; }

            MemoryBlockAllocator *GetNextBlock() { return Next; }

            void Reset() {
                PointerToData = PointerToHead;
                FreeSize = TotalSize;
            };
        };

    }
}