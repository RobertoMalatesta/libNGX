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
            uint64_t RecycleSize = DEFAULT_RECYCLE_SIZE;
        public:
            Recylcer(uint64_t RecycleSize, MemAllocator *Allocator): RecycleSentinel() {
                this -> RecycleSize = RecycleSize;
            }
            Recyclable *Get() {return nullptr;};
            void Put(Recyclable *Item) {};
    };

}