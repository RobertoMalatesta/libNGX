namespace ngx::Http {

    class HttpConnection : public TCP4Connection, public Recyclable {
    protected:
        SpinLock Lock;
        Buffer ReadBuffer;
        HttpRequestContext RequestContext;
        TimerTreeNode TimerNode;
        EventPromiseArgs Arguments;

        static void OnConnectionEvent(void *Arguments, ThreadPool *TPool);

        friend class HttpServer;

        friend class HttpConnectionRecycler;

    public:
        HttpConnection(Core::SocketAddress &SocketAddress, BufferBuilder &BB);

        HttpConnection(int SocketFd, Core::SocketAddress &SocketAddress, BufferBuilder &BB);

        ~HttpConnection() = default;

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


