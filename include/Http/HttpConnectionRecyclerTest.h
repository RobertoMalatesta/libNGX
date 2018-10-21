namespace ngx::Http {

    class HttpConnectionRecycler: public Recycler {
    protected:
        BufferBuilder BB;
        SpinLock Lock;

    public:
        HttpConnectionRecycler(size_t BlockSize, uint64_t BufferRecyclerSize, uint64_t RecyclerSize);
        virtual HttpConnection *Get(int SocketFD, SocketAddress &SocketAddress);
        virtual void Put(HttpConnection *Item);
    };
}