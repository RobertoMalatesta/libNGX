namespace ngx::Core {

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
                };
                uint32_t Flags;
            };

        public:

            Connection(int SocketFD);

            int GetFD(){ return this->SocketFd;};
            int Send(u_char *Buf, size_t Size);
            int Recv(u_char *Buf, size_t Size);
            int SendFile();
            int ReadIOVector();
            int WriteIOVector();

            int Close();
            int SetOption();
    };
}