namespace ngx::Core {

    class Buf {

        private:

            Pool *Allocator;

            u_char *Start, *End;
            u_char *Pos, *Last;

            Buf    *Shadow;

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

        public:
            Buf(Pool * Allocator, size_t Size);
            ~Buf();
    };



}