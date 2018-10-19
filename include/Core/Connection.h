namespace ngx::Core {

    class Connection : public Socket {
    protected:
    public:
        Connection(struct sockaddr *SocketAddress, socklen_t SocketLength);

        Connection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength);

        virtual SocketError Connect() {
            return SocketError(ENOENT, "Method not implemented!");
        };

        virtual SocketError Close() {
            return SocketError(ENOENT, "Method not implemented!");
        };
    };


    class TCP4Connection : public Connection {
    protected:
        Pool Allocator;
    public:
        TCP4Connection(struct sockaddr *SocketAddress, socklen_t SocketLength);

        TCP4Connection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength);

        virtual SocketError Connect();

        virtual SocketError Close();

        SocketError SetNoDelay(bool Open);
    };
}