namespace ngx{
    namespace HTTP {
        struct HTTPPerformanceUnit {
            std::atomic_uint64_t ConnectionCount = {0};
            std::atomic_uint64_t ActiveConnecionCount = {0};
            std::atomic_uint64_t RequestCount = {0};
            std::atomic_uint64_t NormalClosedCount = {0};
            std::atomic_uint64_t ErrorClosedCount = {0};
            std::atomic_uint64_t ReadingCount = {0};
            std::atomic_uint64_t WritingCount = {0};
            std::atomic_uint64_t WatingCount = {0};
        };

        class HTTPServer : public Server {
        protected:

            virtual RuntimeError PostProcessFinished(EventPromiseArgs &Arguments);
            HTTPConnectionBuilder ConnectionBuilder;
            EPollEventDomain EventDomain;
            HTTPPerformanceUnit PerformanceCounters;

        public:
            HTTPServer(size_t PoolSize, int ThreadCount, int EPollSize, size_t BufferBlockSize,
                    uint64_t ConnectionRecycleSize, uint64_t BufferRecycleSize);

            virtual RuntimeError PostConnectionEvent(EventPromiseArgs &Argument);
            virtual RuntimeError CloseConnection(HTTPConnection *&Connection);
            RuntimeError HTTPServerEventProcess();
        };
    }
}
