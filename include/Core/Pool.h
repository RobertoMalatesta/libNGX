namespace ngx::Core {

    static int DefaultPoolResidual = 8;

    class Pool : public MemAllocator{
        private:
            size_t BlockSize;
            MemBlock *HeadBlock, *CurrentBlock;

        public:
            Pool(size_t BlockSize = PageSize);
            ~Pool();
            virtual void *Allocate(size_t Size);
            virtual void Free(void ** pointer);
            void GC(int Residual=DefaultPoolResidual);
            void Reset();
    };
}




