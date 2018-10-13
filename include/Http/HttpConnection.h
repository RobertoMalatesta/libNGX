namespace ngx::Http {


    class HttpConnection: public TCP4Connection, Resetable {
        protected:
            SpinLock Lock;
            HttpRequestBuffer RequestBuffer;
//            HttpResponseBuffer ResponseBuffer;

            static void OnConnectedEvent(void *Arguments, ThreadPool *TPool);
            static void OnReadEvent(void *Arguments, ThreadPool *TPool);
            static void OnWriteEvent(void *Arguments, ThreadPool *TPool);
            static void OnClosedEvent(void *Arguments, ThreadPool *TPool);
            static void TriggerRequestHandler(void *Argument, ThreadPool *TPool) {}
            static void FinalizeRequest(void *Argument, ThreadPool *TPool) {}
            friend class HttpServer;
        public:
            HttpConnection(struct sockaddr *SocketAddress, socklen_t SocketLength, BufferMemoryBlockRecycler *Recycler = nullptr, size_t BufferBlockSize = BUFFER_DEFAULT_BLOCK_SIZE);
            HttpConnection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength, BufferMemoryBlockRecycler *Recycler = nullptr, size_t BufferBlockSize = BUFFER_DEFAULT_BLOCK_SIZE);
            ~HttpConnection() = default;
            inline RuntimeError ReadConnection() {
                return RequestBuffer.WriteDataToBuffer(this);
            }
            RuntimeError ProcessRequest() {
                return RuntimeError(0);
            };
            void Reset();
    };
}

