namespace ngx {
    namespace HTTP {

        class HTTPConnection : protected TCP4Connection, protected Recyclable {
        protected:

            Pool MemPool;

            Timer TimerNode;
            Buffer ReadBuffer;
            HTTPRequest Request;

            HTTPServer *ParentServer;
            SocketEventDomain *ParentEventDomain;

            RuntimeError SetSocketAddress(int SocketFD, struct SocketAddress &TargetSocketAddress);

            static void OnTimerEventWarp(void *PointerToConnection, ThreadPool *TPool);

            static void OnConnectionEvent(void *PointerToConnection, ThreadPool *TPool);

            HTTPConnection(struct SocketAddress &SocketAddress);

            HTTPConnection(int SocketFd, Core::SocketAddress &SocketAddress);

            SocketError Close();

            friend class HTTPServer;

            friend class HTTPConnectionRecycleBin;

            friend class HTTPConnectionBuilder;

        public:

            HTTPConnection();
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


