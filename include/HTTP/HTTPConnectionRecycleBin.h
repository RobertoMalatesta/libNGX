namespace ngx {
    namespace HTTP {

        class HTTPConnectionRecycleBin : public RecycleBin, public AllocatorBuild<HTTPConnection> {
        protected:
            Pool BackendAllocator;
        public:
            HTTPConnectionRecycleBin(uint64_t RecycleBinSize);

            virtual int Get(HTTPConnection *&C, int SocketFD, SocketAddress &TargetSocketAddress);

            virtual int Put(HTTPConnection *&Item);
        };
    }
}