namespace ngx::Core {

    class Listening: public Socket, public Queue {
        public:
            Listening();
            Listening(Core::SocketAddress &SocketAddress, socklen_t SocketLength);
            Listening(int SocketFd, Core::SocketAddress &SocketAddress, socklen_t SocketLength);
            SocketError Listen() {
                return SocketError(ENOENT, "Method not implemented!");
            }
    };

    class TCP4Listening: public Listening {
        protected:
            uint Backlog = 1024;
        public:
            TCP4Listening(Core::SocketAddress &SocketAddress, socklen_t SocketLength);
            ~TCP4Listening();
            SocketError Listen();
            SocketError SetPortReuse(bool Open);
    };
}