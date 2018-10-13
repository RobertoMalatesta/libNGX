namespace ngx::Core {

    class TCP4Listening: public Listening {
        protected:
            uint Backlog = 1024;
        public:
            TCP4Listening(struct sockaddr *SocketAddress, socklen_t SocketLength);
            ~TCP4Listening();
            SocketError Listen();
            SocketError SetPortReuse(bool Open);
    };


    class TCP4Connection: public Connection {
        protected:
            Pool Allocator;
        public:
            TCP4Connection(struct sockaddr* SocketAddress, socklen_t SocketLength);
            TCP4Connection(int SocketFd, struct sockaddr* SocketAddress, socklen_t SocketLength);

            SocketError Connect();
            SocketError Close();
            SocketError SetNoDelay(bool Open);
    };
}