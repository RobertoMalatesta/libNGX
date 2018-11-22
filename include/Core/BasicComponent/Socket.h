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
    int SocketFD = -1;
    SpinLock SocketLock;
    Timer TimerNode;
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

    inline int GetSocketFD() const {
        return SocketFD;
    }

    SocketError SetNonBlock(bool On);

    SocketError SetNoDelay(bool On);

    virtual RuntimeError HandleEventDomain(EventType Type) = 0;


    inline void Lock() {
        SocketLock.Lock();
    }

    inline void Unlock() {
        SocketLock.Unlock();
    }

    inline bool TryLock() {
        return SocketLock.TryLock();
    }

    static inline Socket *TimerToSocket(Timer *T) {
        if (T == nullptr) {
            return nullptr;
        }
        return (Socket *)((uintptr_t)T - (uintptr_t)(&((Socket*)0)->TimerNode));
    }
};
