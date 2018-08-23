namespace ngx::Core {

    class Listen {

        private:
            int SocketFd;
            struct sockaddr SocketAddress;
            socklen_t SocketLength;

            union {
                struct {
                    unsigned Active:1;
                    unsigned Version:3;
                };
                uint32_t Flags;
            };

        public:
            Listen(struct sockaddr SocketAddr, socklen_t SocketLength);

            int Bind(ushort Port) { return EINVAL; };
            Connection *Accept() { return nullptr;};
            int SetOption() { return ENOENT;};
    };

    class TCP4Listen : public Listen{

        public:
            TCP4Listen(struct sockaddr SockAddr, socklen_t SocketLength);
            int Bind(ushort Port);
            Connection *Accept();
            int SetOption();
    };
}