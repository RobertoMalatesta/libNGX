namespace ngx::Core {

    const unsigned long MemBlockMagic = 4268635721590065164;

    class MemBlock: public MemAllocator{
        private:
            int UseCount = 0;
            MemBlock *Next = nullptr;
            size_t TotalSize = 0;
            size_t FreeSize = 0;
            void *PointerToData = nullptr, *PointerToHead = nullptr;
            unsigned long Magic1 = MemBlockMagic, Magic2 = 0;

        public:
            static MemBlock *New(size_t Size = PageSize);
            static void Destroy(MemBlock *MemBlk);
            static MemBlock *AddressToMemBlock(void * Addr, size_t Size);
            MemBlock() = default;
            ~MemBlock();
            bool IsInBlock(void *Address);
            bool IsFreeBlock() { return UseCount <= 0;}
            virtual void *Allocate(size_t Size);
            virtual void Free(void **pointer);
            void GC() {/*Empty Code Block*/};
            void SetNext(MemBlock *Next) {this->Next = Next;}
            MemBlock *GetNext() { return Next;}
            void Reset();
    };
}


