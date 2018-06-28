namespace ngx::Core {

    class Buf {

        private:

            MemAllocator *Allocator = nullptr;
            u_char *Start = nullptr, *End = nullptr, *Pos = nullptr, *Last = nullptr;
            Buf *Shadow = nullptr;

            union {
                struct {
                    unsigned         temporary:1;
                    unsigned         memory:1;
                    unsigned         mmap:1;
                    unsigned         recycled:1;
                    unsigned         in_file:1;
                    unsigned         flush:1;
                    unsigned         sync:1;
                    unsigned         last_buf:1;
                    unsigned         last_in_chain:1;
                    unsigned         last_shadow:1;
                    unsigned         temp_file:1;
                };
                uint32_t flags = 0;
            };

        public:
            Buf(MemAllocator * Allocator, size_t Size);
            ~Buf();
    };
}