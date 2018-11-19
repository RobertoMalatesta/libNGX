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

class Socket : public EventEntity, public Achor{
protected:
    Queue QueueSentinel;
    int SocketFD = -1;
    SocketAddress Address;

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

    friend class SocketEventDomain;

public:
    Socket();

    Socket(struct SocketAddress &SocketAddress);

    Socket(int SocketFD, struct SocketAddress &SocketAddress);

    int GetSocketFD();

    SocketError SetOption() {
        return SocketError(ENOENT, "Method not implemented!");
    }

    inline void AttachSocket(Socket *S) {
        S->QueueSentinel.Append(&QueueSentinel);
    }

    inline void DetachSocket() {
        QueueSentinel.Detach();
    }

    virtual RuntimeError HandleEventDomain(uint32_t EventType) {
        return {ENOENT, "method not implemented!"};
    };
};
