namespace ngx::Http {

    class HttpConnectionRecycler: public Recycler {
    protected:
        SpinLock Lock;

    public:
        HttpConnectionRecycler(uint64_t RecyclerSize): Recycler(RecyclerSize) {
            Lock.Unlock();
        }
        virtual HttpConnection *Get();
        virtual void Put(HttpConnection *Item);
    };
}