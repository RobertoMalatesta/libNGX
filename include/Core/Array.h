namespace ngx::Core {

    class Array {
        private:
            uint NAlloc = 0;
            size_t  Size = 0;
            Pool *Allocator;
            uint ElementCount = 0;
            void *PointerToData;

        public:
            Array(Pool *Allocator, size_t Size, uint Count);
            ~Array();

            void *Push();
            void *PushN(uint N);
    };
}