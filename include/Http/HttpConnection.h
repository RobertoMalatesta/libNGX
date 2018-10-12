namespace ngx::Http {

    /*
     * EPollArguments
     *
     * UserArgument[0] = temp arg0
     * UserArgument[1] = temp arg1
     * UserArgument[2] = temp arg2
     * UserArgument[3] = Server
     * UserArgument[4] = EventDomain
     * UserArgument[5] = Listening
     * UserArgument[6] = Connection
     * UserArgument[7] = EventType
     *
     * */

    class HttpConnection: public TCP4Connection, Resetable {
        protected:
            SpinLock Lock;
            HttpRequestBuffer RequestBuffer;
//            HttpResponseBuffer ResponseBuffer;

            static void OnEventFunc(void *Argument, ThreadPool *TPool);
            static void TriggerRequestHandler(void *Argument, ThreadPool *TPool);
            static void FinalizeRequest(void *Argument, ThreadPool *TPool);
            friend class HttpServer;
        public:
            HttpConnection(struct sockaddr *SocketAddress, socklen_t SocketLength, BufferMemoryBlockRecycler *Recycler = nullptr, size_t BufferBlockSize = BUFFER_DEFAULT_BLOCK_SIZE);
            HttpConnection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength, BufferMemoryBlockRecycler *Recycler = nullptr, size_t BufferBlockSize = BUFFER_DEFAULT_BLOCK_SIZE);
            ~HttpConnection();
            inline RuntimeError ReadConnection() {
                return RequestBuffer.WriteDataToBuffer(this);
            }
            RuntimeError ProcessRequest();
            HttpConnection *Build( MemAllocator *Allocator, int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength);
            void Destroy(MemAllocator *Allocator, HttpConnection **HttpConnection);
            void Reset();
    };
}


