namespace ngx::Core {

    static int PoolReserveMemBlockCount = 1;

    class Pool : public MemAllocator{
        private:
            size_t BlockSize;
            MemBlock *HeadBlock, *CurrentBlock;

        public:
            Pool(size_t BlockSize = PageSize);
            ~Pool();
            void *Allocate(size_t Size);
            void Free(void * pointer);
            void GC();
            void Reset();
    };
}




