namespace ngx::Http {

    class HttpConnectionRecycler: public Recycler {
    protected:
        SpinLock Lock;
        MemAllocator *Allocator;

    public:
        HttpConnectionRecycler(uint64_t RecyclerSize);
        virtual HttpConnection *Get(int SocketFD, SocketAddress &SocketAddress, socklen_t SocketLength, Buffer &Buf);
        virtual void Put(HttpConnection *Item);
    };
}