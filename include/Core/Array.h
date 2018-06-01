namespace ngx::Core {

    class Array {

        private:
            Pool *Allocator;

            void *PointerToData;
            uint ElementCount;
            size_t  Size;
            uint NAlloc;

        public:
            Array(Pool *Allocator, size_t Size, int Count);
            ~Array();

            void *Push();
            void *PushN(uint N);
    };
}