namespace ngx{
    namespace HTTP {

        class HTTPConnectionRecycler : public Recycler {
        protected:
            BufferBuilder BB;
            SpinLock Lock;

        public:
            HTTPConnectionRecycler(size_t BlockSize, uint64_t BufferRecyclerSize, uint64_t RecyclerSize);

            virtual HTTPConnection *Get(int SocketFD, SocketAddress &SocketAddress);

            virtual void Put(HTTPConnection *Item);
        };
    }
}