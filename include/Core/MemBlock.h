namespace ngx::Core {
	static size_t PageSize = 4096;

    const unsigned long MemBlockMagic = 4268635721590065164;

	class MemBlock: public MemAllocator{
	    private:
            int UseCount;
            MemBlock *Next;
            size_t TotalSize;
            size_t FreeSize;
            void *PointerToData, *PointerToHead;
            unsigned long Magic1, Magic2;

	    public: 
			static MemBlock *New(size_t Size = PageSize);
			static void Destroy(MemBlock *MemBlk);
			static MemBlock *AddressToMemBlock(void * Addr, size_t Size);
            bool IsInBlock(void *Address);
            bool IsFreeBlock() { return UseCount <= 0;}
            void *Allocate(size_t Size);
            void Free(void *pointer);
			void GC() {/*Empty Code Block*/};
            void SetNext(MemBlock *Next) {this->Next = Next;}
            MemBlock *GetNext() { return Next;}
			void Reset();
	};
}


