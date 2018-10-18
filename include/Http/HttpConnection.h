namespace ngx::Http {


    class HttpConnection: public TCP4Connection, Resetable {
        protected:
            SpinLock Lock;
            Buffer ReadBuffer;
	    HttpRequestContext RequestBuffer;
//            HttpResponseBuffer ResponseBuffer;
            static void OnConnectionEvent(void *Arguments, ThreadPool *TPool);
            static void TriggerRequestHandler(void *Argument, ThreadPool *TPool) {}
            static void FinalizeRequest(void *Argument, ThreadPool *TPool) {}
            friend class HttpServer;
        public:
            HttpConnection(struct sockaddr *SocketAddress, socklen_t SocketLength, BufferMemoryBlockRecycler *Recycler = nullptr, size_t BufferBlockSize = BUFFER_DEFAULT_BLOCK_SIZE);
            HttpConnection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength, BufferMemoryBlockRecycler *Recycler = nullptr, size_t BufferBlockSize = BUFFER_DEFAULT_BLOCK_SIZE);
            ~HttpConnection() = default;
            inline RuntimeError ReadConnection() {
                return ReadBuffer.WriteConnectionToBuffer(this);
            }
            RuntimeError ProcessRequest() {
                return RuntimeError(0);
            };
            void Reset();
    };
}


