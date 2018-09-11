namespace ngx::Core {

    static void DiscardPromise(void *Argument, ThreadPool *TPool) {
        return;
    }

    class Connection {

        private:

            int SocketFd;
            struct sockaddr SocketAddress;
            socklen_t SocketLength;

            union {
                struct {
                    unsigned Timeout:1;
                    unsigned Error:1;
                    unsigned Destoryed:1;
                    unsigned Idle:1;
                    unsigned Reuseable:1;
                    unsigned Closed:1;
                    unsigned Shared:1;
                    unsigned SendLowat:1;
                    unsigned Tcp_nodelay:1;
                    unsigned Tcp_nopush:1;
                    unsigned Attached;
                };
                uint32_t Flags;
            };

            PromiseCallback  *OnRead = &DiscardPromise;
            PromiseCallback  *OnWrite = &DiscardPromise;
            PromiseCallback  *OnClose = &DiscardPromise;

        public:
            Connection(int SocketFD);
            int GetFD(){ return this->SocketFd;};
            int Close();
            int SetOption();
    };
}