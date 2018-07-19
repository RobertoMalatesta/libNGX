namespace ngx::Core {

    class MemBlock: public MemAllocator{
        private:
            std::atomic<int> UseCount = 0;
            MemBlock *Next = nullptr;
            size_t TotalSize = 0;
            size_t FreeSize = 0;
            void *PointerToData = nullptr, *PointerToHead = nullptr;
            unsigned long Magic = 0;

        public:
            MemBlock(size_t Size);
            
            static MemBlock *CreateMemBlock(size_t Size);
            static void FreeMemBlock(MemBlock **PointerToBlock);

            static MemBlock *AddressToMemBlock(void * Addr, size_t Size);
            ~MemBlock();
            bool IsInBlock(void *Address);
            bool IsFreeBlock() { return UseCount <= 0;}
            virtual void *Allocate(size_t Size);
            virtual void Free(void **pointer);
            virtual void GC() {/*Empty Code Block*/};
            void SetNext(MemBlock *Next) {this->Next = Next;}
            MemBlock *GetNext() { return Next;}
            void Reset();
    };
}


