namespace ngx::Core {

    class Listening {

        protected:
            int SocketFd;
            struct sockaddr *SocketAddress;
            socklen_t SocketLength;

            union {
                struct {
                    unsigned Active:1;
                    unsigned Version:3;
                    unsigned Stream:1;
                };
                u_short Flags;
            };

        public:
            Listening(struct sockaddr *SocketAddr);

            int GetSocketFD() {return this->SocketFd;}
            int Bind();
            int SetOption() { return ENOENT;};
    };

    class TCP4Listening : public Listening{
        private:
            union {
                struct {
                    unsigned Reuse:1;
                };
                u_short TCPFlags;
            };

        public:
            TCP4Listening(struct sockaddr *SockAddr, socklen_t SocketLength);
            ~TCP4Listening();

            SocketError Bind();
            SocketError SetPortReuse(bool Open);
    };
}