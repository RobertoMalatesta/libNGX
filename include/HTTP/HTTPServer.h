namespace ngx {
    namespace HTTP {

        struct HTTPPerformanceUnit {
            std::atomic_uint64_t ProcessRound = {0};
            std::atomic_uint64_t ConnectionCount = {0};
            std::atomic_uint64_t ActiveConnecionCount = {0};
            std::atomic_uint64_t RequestCount = {0};
            std::atomic_uint64_t NormalClosedCount = {0};
            std::atomic_uint64_t ErrorClosedCount = {0};
            std::atomic_uint64_t ReadingCount = {0};
            std::atomic_uint64_t WritingCount = {0};
            std::atomic_uint64_t WatingCount = {0};
        };

        class HTTPServer : protected Server {
        protected:
            HTTPConnectionBuilder ConnectionBuilder;
            EPollEventDomain EventDomain;
            HTTPPerformanceUnit PerformanceCounters;

            virtual RuntimeError PostProcessFinished();

        public:
            HTTPServer(size_t PoolSize, int ThreadCount, int EPollSize, size_t BufferBlockSize,
                       uint64_t ConnectionRecycleSize, uint64_t BufferRecycleSize);

            EventError EnqueueListening(HTTPListening *L);

            EventError DequeueListening(HTTPListening *&L);

            EventError AttachConnection(HTTPConnection &C);

            EventError DetachConnection(HTTPConnection &C);

            virtual RuntimeError GetConnection(HTTPConnection *&C, int SocketFD, SocketAddress &Address);

            virtual RuntimeError PutConnection(HTTPConnection *&C);


            RuntimeError HTTPServerEventProcess();

            ~HTTPServer() = default;
        };
    }
}
