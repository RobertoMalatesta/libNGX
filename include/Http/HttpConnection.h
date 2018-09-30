namespace ngx::Http {

    /*
     * EPollArguments
     *
     * UserArgument[0] = EpollEventDomain
     * UserArgument[1] = HttpConnection
     * UserArgument[2] = EventType
     *
     * */

    class HttpConnection: public TCP4Connection, Resetable {
        protected:
            SpinLock Lock;
//            HttpRequestBuffer RequestBuffer;
//            HttpResponseBuffer ResponseBuffer;
            static void OnEventFunc(void *Argument, ThreadPool *TPool);
            static void TriggerRequestHandler(void *Argument, ThreadPool *TPool);
            static void FinalizeRequest(void *Argument, ThreadPool *TPool);
            friend class HttpServer;
        public:
            HttpConnection(struct sockaddr *SocketAddress, socklen_t SocketLength);
            HttpConnection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength);
            ~HttpConnection();
            void Reset();
            HttpConnection *Build( MemAllocator *Allocator, int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength);
            void Destroy(MemAllocator *Allocator, HttpConnection **HttpConnection);
    };
}


