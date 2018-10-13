namespace ngx::Http {

    struct HttpPerformanceUnit {
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

            virtual RuntimeError PostProcessFinished(EventPromiseArgs *Arguments);
            virtual RuntimeError PostNewConnection(EventPromiseArgs *Arguments);
            virtual RuntimeError PostConnectionRead(EventPromiseArgs *Arguments);
            virtual RuntimeError PostConnectionWrite(EventPromiseArgs *Arguments);
            virtual RuntimeError PostConnectionClosed(EventPromiseArgs *Arguments);

            EPollEventDomain EventDomain;
            HttpPerformanceUnit PerformanceCounters;

        public:
            HttpServer(size_t PoolSize, int ThreadCount, int EPollSize, int ConnectionRecycleSize, int BufferRecycleSize);
            RuntimeError HttpServerEventProcess();
    };
}
