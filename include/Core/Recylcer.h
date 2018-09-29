namespace ngx::Core {

    const uint64_t DEFAULT_RECYCLE_SIZE = 1024;

    class Resetable {
        public:
            void Reset() {};
    };

    class Recyclable : public Resetable, public Queue {
        public:
            Recyclable(): Queue() {};
    };

    class Recylcer {
        protected:
            Recyclable RecycleSentinel;
            uint64_t RecycleMaxSize = DEFAULT_RECYCLE_SIZE, RecycleSize = 0 ;
        public:
            Recylcer(uint64_t RecycleMaxSize, MemAllocator *Allocator): RecycleSentinel() {
                this -> RecycleMaxSize = RecycleMaxSize;
            }
            Recyclable *Get() {return nullptr;};
            void Put(Recyclable *Item) {};
    };

}