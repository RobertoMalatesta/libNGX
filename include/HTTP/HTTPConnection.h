namespace ngx{
    namespace HTTP {

        class HTTPConnection : public TCP4Connection, public Recyclable {
        protected:
            SpinLock Lock;
            Pool MemPool;
            Buffer ReadBuffer;
            TimerTreeNode TimerNode;
            EventPromiseArgs Arguments;
            HTTPRequest CurrentRequest;
            PromiseCallback *OnEventPromise = HTTPConnection::OnConnectionEvent;

            static void OnConnectionEvent(void *Arguments, ThreadPool *TPool);

            friend class HTTPServer;

            friend class HTTPConnectionRecycler;

        public:
            HTTPConnection(Core::SocketAddress &SocketAddress, BufferBuilder &BB);

            HTTPConnection(int SocketFd, Core::SocketAddress &SocketAddress, BufferBuilder &BB);

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


