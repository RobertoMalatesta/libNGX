namespace ngx::Core {

    class Recycler {
    protected:
        Recyclable RecycleSentinel;
        uint64_t RecycleMaxSize = DEFAULT_RECYCLE_SIZE, RecycleSize = 0;
    public:
        Recycler(uint64_t RecycleMaxSize) : RecycleSentinel() {
            this->RecycleMaxSize = RecycleMaxSize;
        }

        virtual Recyclable *Get() { return nullptr; };

        virtual void Put(Recyclable *Item) {};
    };

}