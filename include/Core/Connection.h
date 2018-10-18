namespace ngx::Core {

    class Connection: public Socket, public Queue{
        public:
            Connection(struct sockaddr *SocketAddress, socklen_t SocketLength);
            Connection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength);
            SocketError Connect() {
                return SocketError(ENOENT, "Method not implemented!");
            };
            SocketError Close() {
                return SocketError(ENOENT, "Method not implemented!");
            };
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