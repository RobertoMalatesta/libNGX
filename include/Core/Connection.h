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
            PromiseCallback  *OnAccept = &DiscardPromise;
            PromiseCallback  *OnRead = &DiscardPromise;
            PromiseCallback  *OnWrite = &DiscardPromise;
            PromiseCallback  *OnClose = &DiscardPromise;
    };

    class Socket : protected EventEntity{
        protected:
            int SocketFd;
            struct sockaddr *SocketAddress;
            socklen_t SocketLength;
            Queue SocketPeer;
            atomic_flag EventLock = ATOMIC_FLAG_INIT;
            union {
                struct {
                    unsigned Open: 1;
                    unsigned Active: 1;
                    unsigned Type: 2;
                    unsigned Expired:1;
                    unsigned IsListen:1;
                    unsigned Version:3;
                    unsigned Reuse:1;
                    unsigned Nodelay:1;
                };
                u_short Flags = 0;
            };

        public:
            Socket (int SocketFd);
            Socket(struct sockaddr *SocketAddress, socklen_t SocketLength);
            int GetSocketFD();
            SocketError SetOption() {
                return SocketError(ENOENT, "Method not implemented!");
            }
    };

    class Listening: public Socket, public Queue {
        public:
            Listening(int SocketFd);
            Listening(struct sockaddr *SocketAddress, socklen_t SocketLength);
            SocketError Listen() {
                return SocketError(ENOENT, "Method not implemented!");
            }
    };

    class TCP4Listening: public Listening {
        protected:
            uint Backlog = 1024;
        public:
            TCP4Listening(struct sockaddr *SocketAddress, socklen_t SocketLength);
            ~TCP4Listening();
            SocketError Listen();
            SocketError SetPortReuse(bool Open);
    };

    class Connection: public Socket, public Queue{
        public:
            Connection(int SocketFd);
            Connection(struct sockaddr *SocketAddress, socklen_t SocketLength);
            SocketError Connect() {
                return SocketError(ENOENT, "Method not implemented!");
            };
            SocketError Close() {
                return SocketError(ENOENT, "Method not implemented!");
            };
    };

    class TCP4Connection: public Connection {
        public:
            TCP4Connection(int SocketFd);
            TCP4Connection(struct sockaddr* SocketAddress, socklen_t SocketLength);
            SocketError Connect();
            SocketError Close();
            SocketError SetNoDelay(bool Open);
    };
}