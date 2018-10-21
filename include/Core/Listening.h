namespace ngx::Core {

    class Listening: public Socket, public Queue {
        public:
            Listening();
            Listening(struct SocketAddress &SocketAddress);
            Listening(int SocketFd, struct SocketAddress &SocketAddress);
            virtual SocketError Listen() {
                return SocketError(EINVAL, "Method not implement!");
            };
    };

    class TCP4Listening: public Listening {
        protected:
            uint Backlog = 1024;
        public:
            TCP4Listening(struct SocketAddress &SocketAddress);
            ~TCP4Listening();
            virtual SocketError Listen();
            SocketError SetPortReuse(bool Open);
    };
}