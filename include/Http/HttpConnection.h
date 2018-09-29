namespace ngx::Http {
    class HttpConnection: public TCP4Connection, Resetable {
        protected:
            SpinLock Lock;
            // BufferQueue
            // RequestQueue
            static void OnEventFunc(void *Argument, ThreadPool *TPool);
            friend class HttpServer;

        public:
            HttpConnection(struct sockaddr *SocketAddress, socklen_t SocketLength);
            HttpConnection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength);
            void Reset();
    };
}


