namespace ngx::Http {

    class HttpConnection : public TCP4Connection, public Recyclable {
    protected:
        SpinLock Lock;
        Buffer ReadBuffer;

        static void OnConnectionEvent(void *Arguments, ThreadPool *TPool);
        friend class HttpServer;

    public:
        HttpConnection(Core::SocketAddress *SocketAddress, socklen_t SocketLength, Buffer Buf);

        HttpConnection(int SocketFd, Core::SocketAddress *SocketAddress, socklen_t SocketLength, Buffer Buf);

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


