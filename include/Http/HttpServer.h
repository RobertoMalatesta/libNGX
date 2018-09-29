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

    struct HttpPerformaceUnit {
        atomic_uint64_t ConnectionCount = {0};
        atomic_uint64_t ActiveConnecionCount = {0};
        atomic_uint64_t RequestCount = {0};
        atomic_uint64_t NormalClosedCount = {0};
        atomic_uint64_t ErrorClosedCount = {0};
        atomic_uint64_t ReadingCount = {0};
        atomic_uint64_t WritingCount = {0};
        atomic_uint64_t WatingCount = {0};
    };


    class HttpServer : public Server {
        protected:
            static void HttpEventProcessPromise(void *Args, ThreadPool *TPool);

            EPollEventDomain EventDomain;
            HttpPerformaceUnit PerformaceCounters;
        public:
            HttpServer(size_t PoolSize, int ThreadCount, int EPollSize, int ConnectionRecycleSize, int BufferRecycleSize);
            RuntimeError HttpServerEventProcess();
    };
}
