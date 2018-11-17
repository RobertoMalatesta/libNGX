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

            HTTPConnection(struct SocketAddress &SocketAddress);

            HTTPConnection(int SocketFD, SocketAddress &SocketAddress);

            RuntimeError SetSocketAddress(int SocketFD, struct SocketAddress &Address);

            virtual RuntimeError HandleEventDomain(uint32_t EventType);

            static void OnTimerEventWarp(void *PointerToConnection, ThreadPool *TPool);

            static void OnConnectionEvent(void *PointerToConnection, ThreadPool *TPool);

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

            SocketError Close();

            virtual void Reset();
        };
    }
}


