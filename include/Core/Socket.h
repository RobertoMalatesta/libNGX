namespace ngx{
    namespace Core {
        enum {
            SOCK_TYPE_DGRAM = 0,
            SOCK_TYPE_STREAM = 1,
            SOCK_TYPE_RESERVED1,
            SOCK_TYPE_RESERVED2
        };

        struct SocketAddress {
            socklen_t SocketLength;
            union {
                struct sockaddr sockaddr;
                struct sockaddr_in sockaddr_in;
                struct sockaddr_in6 sockaddr_in6;
            };
        };

        class Socket : protected EventEntity {
        protected:
            Queue QueueSentienl;
            int SocketFd = -1;
            SocketAddress SocketAddress;
            std::atomic_flag EventLock = ATOMIC_FLAG_INIT;
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
                    unsigned ReadAttach:1;
                    unsigned WriteAttach:1;
                };
                u_short Flags = 0;
            };

            friend class SocketEventDomain;

        public:
            Socket();

            Socket(struct SocketAddress &SocketAddress);

            Socket(int SocketFd, struct SocketAddress &SocketAddress);

            int GetSocketFD();

            SocketError SetOption() {
                return SocketError(ENOENT, "Method not implemented!");
            }

            inline void AttachSocket(Socket *S) {
                S->QueueSentienl.Append(&QueueSentienl);
            }

            inline void DetachSocket() {
                QueueSentienl.Detach();
            }

            virtual void Achor() {};
        };
    }
}

