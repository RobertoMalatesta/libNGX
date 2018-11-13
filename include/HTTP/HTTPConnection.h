namespace ngx{
    namespace HTTP {

        class HTTPConnection : public TCP4Connection, public Recyclable {
        protected:
            bool Closed = true;
            uint64_t LastEventTimestamp = 0;

            Pool MemPool;
            SpinLock Lock;
            Timer TimerNode;
            EventType Event;
            Buffer ReadBuffer;
            HTTPRequest Request;
            HTTPServer *ParentServer;
            Listening *ParentListeing;
            SocketEventDomain *ParentEventDomain;

            PromiseCallback *OnEventPromise = HTTPConnection::OnConnectionEvent;

            RuntimeError SetSocketAddress(int SocketFD, struct SocketAddress &TargetSocketAddress);

            static void OnTimerEventWarp(void *PointerToConnection, ThreadPool *TPool);
            static void OnConnectionEvent(void *PointerToConnection, ThreadPool *TPool);

            HTTPConnection(struct SocketAddress &SocketAddress);
            HTTPConnection(int SocketFd, Core::SocketAddress &SocketAddress);

            SocketError Close();
            RuntimeError RefreshTimer();

            friend class HTTPServer;
            friend class HTTPConnectionRecycler;
            friend class HTTPConnectionBuilder;
        public:

            ~HTTPConnection() = default;

            inline RuntimeError ReadConnection() {
                return ReadBuffer.WriteConnectionToBuffer(this);
            }

            inline RuntimeError WriteConnection() {
                return RuntimeError(EINVAL);
            }

            RuntimeError ProcessRequest() {
                return RuntimeError(0);
            };

            virtual void Reset();
        };
    }
}


