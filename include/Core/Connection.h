namespace ngx::Core {

    enum {
        SOCK_TYPE_DGRAM = 0,
        SOCK_TYPE_STREAM = 1,
        SOCK_TYPE_RESERVED1,
        SOCK_TYPE_RESERVED2
    };

    static void DiscardPromise(void *Argument, ThreadPool *TPool) {
        return;
    }

    class EventEntity {
        protected:
            PromiseCallback  *OnRead = &DiscardPromise;
            PromiseCallback  *OnWrite = &DiscardPromise;
            PromiseCallback  *OnClose = &DiscardPromise;
    };

    class Socket : protected EventEntity{
        protected:
            int SocketFd;
            struct sockaddr *SocketAddress;
            socklen_t SocketLength;

            union {
                struct {
                    unsigned Open: 1;
                    unsigned Active: 1;
                    unsigned Type: 2;
                    unsigned Expired:1;
                    unsigned IsListen:1;
                    unsigned Version:3;
                    unsigned Reuse;
                };
                u_short Flags = 0;
            };

        public:
            Socket (int SocketFd);
            Socket(struct sockaddr *SocketAddress, socklen_t SocketLength);
            int GetSocketFD();
            SocketError SetOption();
    };

    class Connection: Socket {
        public:
            Connection(int SocketFd);
            SocketError Connect();
            SocketError Close();
    };

    class Listening: protected Socket {
        public:
            Listening(int SocketFd);
            Listening(struct sockaddr *SocketAddress, socklen_t SocketLength);
            SocketError Listen();
    };

    class TCP4Listening: protected Listening {

    public:
        TCP4Listening(struct sockaddr *SocketAddress, socklen_t SocketLength);
        ~TCP4Listening();
        SocketError Bind();
        SocketError Listen();
        SocketError SetPortReuse(bool Open);
    };
}