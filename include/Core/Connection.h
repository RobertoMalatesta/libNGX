namespace ngx::Core {

    class Connection {

        private:
            struct sockaddr SocketAddress;
            socklen_t SocketLength;

            int SocketFd;

            unsigned Timeout:1;
            unsigned Error:1;
            unsigned Destoryed:1;
            unsigned Idle:1;
            unsigned Reuseable:1;
            unsigned Close:1;
            unsigned Shared:1;
            unsigned SendLowat:1;
            unsigned Tcp_nodelay:1;
            unsigned Tcp_nopush:1;

        public:

            size_t Recv(u_char *Buf, size_t Size);
            size_t Send(u_char *Buf, size_t Size);
            void CloseConnection();
    };
}