namespace ngx{
    namespace HTTP {

        class HTTPConnection : public TCP4Connection, public Recyclable {
        protected:
            SpinLock Lock;
            Pool MemPool;
            Buffer ReadBuffer;
            TimerTreeNode TimerNode;
            HTTPRequest Request;

            EventType Event;
            HTTPServer *ParentServer;
            Listening *ParentListeing;
            SocketEventDomain *ParentEventDomain;

            PromiseCallback *OnEventPromise = HTTPConnection::OnConnectionEvent;

            RuntimeError SetSocketAddress(int SocketFD, struct SocketAddress &TargetSocketAddress);

            static void OnCloseConnection(void *PointerToConnection, ThreadPool *TPool);
            static void OnConnectionEvent(void *PointerToConnection, ThreadPool *TPool);

            HTTPConnection(Core::SocketAddress &SocketAddress);
            HTTPConnection(int SocketFd, Core::SocketAddress &SocketAddress);

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


