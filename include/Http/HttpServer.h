namespace ngx::Http {

    typedef struct _HttpEventPromiseArguments {
       HttpServer *Server;
       HttpConnection *Connection;
    } HttpEventPromiseArguments;

    class HttpConnection: public TCP4Connection {
        protected:
            SpinLock Lock;
            // BufferQueue
            // RequestQueue
            static void OnHttpConnectionConnected(void *Arguments, ThreadPool *TPool);
            static void OnHttpConnectionRead(void *Arguments, ThreadPool *TPool);
            static void OnHttpConnectionWrite(void *Arguments, ThreadPool *TPool);
            static void OnHttpConnectionClose(void *Arguments, ThreadPool *TPool);
            friend class HttpServer;

        public:
            HttpConnection(struct sockaddr *SocketAddress, socklen_t SocketLength);
            HttpConnection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength);
    };

    class HttpServer : public EPollEventDomain{
        protected:
            static void HttpEventProcessPromise(void *Args, ThreadPool *TPool);

            Pool Allocator;
            struct {
                atomic_uint64_t ConnectionCount;
                atomic_uint64_t ActiveConnecionCount;
                atomic_uint64_t RequestCount;
                atomic_uint64_t NormalClosedCount;
                atomic_uint64_t ErrorClosedCount;
                atomic_uint64_t ReadingCount;
                atomic_uint64_t WritingCount;
                atomic_uint64_t WatingCount;
            };
        public:
            HttpServer(size_t PoolSize, int ThreadCount, int EPollSize, int ConnectionRecycleSize, int BufferRecycleSize);
    };
}
