namespace ngx::Core {

    static int DefaultPoolResidual = 8;

    class Pool : public MemAllocator, Recyclable{
        private:
            size_t BlockSize;
            MemoryBlockAllocator *HeadBlock, *CurrentBlock;

        public:
            Pool(size_t BlockSize = PageSize);
            ~Pool();
            virtual void *Allocate(size_t Size);
            virtual void Free(void ** pointer);
            virtual void GC();
            void Reset();
    };
}




