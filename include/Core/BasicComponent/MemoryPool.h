#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H
namespace ngx {
    namespace Core {
        namespace BasicComponent {
            class MemoryPool {
            protected:
                class PoolMemoryBlock {
                    int RefCount;
                    Byte *Start, *End, *Pos;

                protected:
                    PoolMemoryBlock():RefCount(0), Start(nullptr), End(nullptr), Pos(nullptr) {};

                public:
                    PoolMemoryBlock(PoolMemoryBlock&) = delete;
                    PoolMemoryBlock operator=(PoolMemoryBlock&) = delete;

                    static std::unique_ptr<PoolMemoryBlock> newBlock(size_t BlockSize=POOL_MEMORY_BLOCK_SIZE);

                    Byte *allocate(size_t Size);
                    void free(void *&Pointer);
                    bool isFreeBlock();
                };

                size_t BlockSize;
                std::list<std::unique_ptr<PoolMemoryBlock>> BlockList;
                std::list<std::unique_ptr<PoolMemoryBlock>>::iterator CurrentBlock;

            public:
                MemoryPool(size_t BlockSize=POOL_MEMORY_BLOCK_SIZE);
                void *allocate(size_t Size);
                void free(void *&Pointer);
                void collect();
            };
        } // BasicComponent
    }   // Core
}   // ngx

#endif //MEMORYPOOL_H
