namespace ngx::Core {
    class MemoryBlock{
        protected:
            std::atomic_uint UseCount = {0};
            size_t TotalSize = 0;
            size_t FreeSize = 0;
            void *PointerToData = nullptr, *PointerToHead = nullptr;
            void *Magic = nullptr;
        public:
            MemoryBlock(size_t Size);
            ~MemoryBlock();

            static MemoryBlock *Build(size_t Size);
            static void Destroy(MemoryBlock **PointerToBlock);

            static MemoryBlock *AddressToMemoryBlock(void *Address, size_t Size);

            bool IsInBlock(void *Address);
            bool IsFreeBlock() { return UseCount == 0; }
    };
}