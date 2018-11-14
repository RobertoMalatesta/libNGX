namespace ngx {
    namespace HTTP {

        class HTTPConnectionRecycleBin : public RecycleBin {
        protected:
        public:
            HTTPConnectionRecycleBin(uint64_t RecycleBinSize);

            virtual int Get(HTTPConnection *&C, int SocketFD, SocketAddress &TargetSocketAddress);

            virtual int Put(HTTPConnection *&Item);
        };
    }
}