namespace ngx::Core {
    class Pool : public MemAllocator{
        private:
            size_t BlockSize;
            MemBlock *HeadBlock, *CurrentBlock;

        public:

            Pool(size_t BlockSize = PageSize);
            void *Allocate(size_t Size);
            void Free(void * pointer);
    };
}




