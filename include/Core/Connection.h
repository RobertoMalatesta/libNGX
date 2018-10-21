namespace ngx::Core {

    class Connection : public Socket {
    protected:
    public:
        Connection();

        Connection(struct SocketAddress &SocketAddress);

        Connection(int SocketFd, struct SocketAddress &SocketAddress);

        virtual SocketError Connect() {
            return SocketError(EINVAL, "Method not implement!");
        };

        virtual SocketError Close() {
            return SocketError(EINVAL, "Method not implement!");
        };
    };


    class TCP4Connection : public Connection {
    public:
        TCP4Connection(struct SocketAddress &SocketAddress);

        TCP4Connection(int SocketFd, struct SocketAddress &SocketAddress);

        virtual SocketError Connect();

        virtual SocketError Close();

        SocketError SetNoDelay(bool Open);
    };
}