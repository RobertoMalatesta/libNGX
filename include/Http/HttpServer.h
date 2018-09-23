namespace ngx::Http {

    struct HttpEventArgument : EPollEventDomainArgument{
        HttpServer *Server;
        HttpConnection *Connection;
        union {
            struct {
                unsigned Read:1;
                unsigned Write:1;
                unsigned Connected:1;
                unsigned Closed:1;
            };
            unsigned char Flags;
        };
    };

    class HttpConnection: public TCP4Connection {
        protected:
            SpinLock Lock;
            // BufferQueue
            // RequestQueue
            static void OnEventFunc(void *Argument, ThreadPool *TPool);
            friend class HttpServer;

        public:
            HttpConnection(struct sockaddr *SocketAddress, socklen_t SocketLength);
            HttpConnection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength);
    };

    class HttpServer : public Server {
        protected:
            EPollEventDomain EventDomain;
            static void HttpEventProcessPromise(void *Args, ThreadPool *TPool);

            struct {
                atomic_uint64_t ConnectionCount = {0};
                atomic_uint64_t ActiveConnecionCount = {0};
                atomic_uint64_t RequestCount = {0};
                atomic_uint64_t NormalClosedCount = {0};
                atomic_uint64_t ErrorClosedCount = {0};
                atomic_uint64_t ReadingCount = {0};
                atomic_uint64_t WritingCount = {0};
                atomic_uint64_t WatingCount = {0};
            };
        public:
            HttpServer(size_t PoolSize, int ThreadCount, int EPollSize, int ConnectionRecycleSize, int BufferRecycleSize);
            RuntimeError HttpServerEventProcess();
    };
}
