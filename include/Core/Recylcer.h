namespace ngx::Core {

    class Resetable {
        public:
            virtual void Reset() = 0;
    };

    class Recyclable : public Resetable, public Queue {
        public:
            Recyclable(): Queue() {};
            virtual void Reset() {};
    };

    class Recycler {
        protected:
            Recyclable RecycleSentinel;
            uint64_t RecycleMaxSize = DEFAULT_RECYCLE_SIZE, RecycleSize = 0 ;
        public:
            Recycler(uint64_t RecycleMaxSize, MemAllocator *Allocator): RecycleSentinel() {
                this -> RecycleMaxSize = RecycleMaxSize;
            }
            Recyclable *Get() {return nullptr;};
            void Put(Recyclable *Item) {};
    };

}