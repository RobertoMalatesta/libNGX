namespace ngx::Http {

    class HttpListening: public  TCP4Listening {

    };

    class HttpConnection: public TCP4Connection {

    };

    class HttpServer {
        protected:
            static void HttpEventProcessPromise(void *Args, ThreadPool *TPool);
            EPollEventDomain Domain;
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
