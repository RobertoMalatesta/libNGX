namespace ngx {
    namespace HTTP {

        class HTTPConnectionRecycler : public Recycler {
        protected:
        public:
            HTTPConnectionRecycler(uint64_t RecyclerSize);

            virtual HTTPConnection *Get(int SocketFD, SocketAddress &TargetSocketAddress);

            virtual void Put(HTTPConnection *Item);
        };
    }
}