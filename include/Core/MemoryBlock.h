namespace ngx{
    namespace Core {
        class MemoryBlock : public Reference, public BuildAlign<MemoryBlock> {
        protected:
            size_t TotalSize = 0;
            size_t FreeSize = 0;
            void *PointerToData = nullptr, *PointerToHead = nullptr;
            void *Magic = nullptr;
        public:
            MemoryBlock(size_t Size);

            ~MemoryBlock();

            static MemoryBlock *AddressToMemoryBlock(void *Address, size_t Size);

            bool IsInBlock(void *Address);

            bool IsFreeBlock() { return RefCount() == 0; }
        };

    }
}