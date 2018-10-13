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
        private:

            static void HttpOnEventProcessFinished(void *Args, ThreadPool *TPool);
            static void HttpOnNewConnection(void *Args, ThreadPool *TPool);
            static void HttpOnConnectionRead(void *Args, ThreadPool *TPool);
            static void HttpOnConnectionWrite(void *Args, ThreadPool *TPool);
        protected:

            static void HttpEventProcessPromise(void *Args, ThreadPool *TPool);

            virtual RuntimeError PostProcessFinished(void *Arguments);
            virtual RuntimeError PostNewConnection(void *Arguments);
            virtual RuntimeError PostConnectionRead(void *Arguments);
            virtual RuntimeError PostConnectionWrite(void *Arguments);

            EPollEventDomain EventDomain;
            HttpPerformanceUnit PerformanceCounters;

        public:
            HttpServer(size_t PoolSize, int ThreadCount, int EPollSize, int ConnectionRecycleSize, int BufferRecycleSize);
            RuntimeError HttpServerEventProcess();
    };
}
