class RecycleBin {
protected:
    Queue RecycleSentinel;
    uint64_t RecycleMaxSize = DEFAULT_RECYCLE_SIZE, RecycleSize = 0;
public:
    RecycleBin(uint64_t RecycleMaxSize) : RecycleSentinel() {
        this->RecycleMaxSize = RecycleMaxSize;
    }

    virtual Recyclable *Get() { return nullptr; };

    virtual void Put(Recyclable *Item) {};
};
