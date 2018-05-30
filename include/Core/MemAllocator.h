namespace ngx::Core {

    class MemAllocator {
        public:
            void *Allocate(size_t Size);
            void Free(void * Pointer);
            void GC();
    };
}