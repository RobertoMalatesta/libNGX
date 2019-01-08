
union SocketAddress {

    struct sockaddr sockaddr;
    struct sockaddr_in sockaddr_in;
    struct sockaddr_in6 sockaddr_in6;

    SocketAddress() {
        sockaddr = {0};
    }

    inline char IPVersion() const {

        if (sockaddr.sa_family & AF_INET) {
            return 4;
        } else if (sockaddr.sa_family & AF_INET6) {
            return 6;
        }

        return -1;
    }
};

class Socket : public EventEntity, public Achor {
protected:

    int SocketFD = -1;

    SpinLock SocketLock;

    SocketAddress Address;

    Timer TimerNode;

    friend class SocketEventDomain;

public:

    Socket();

    Socket(SocketAddress &Address);

    Socket(int SocketFD, SocketAddress &Address);

    inline int GetSocketFD() const {
        return SocketFD;
    }

    inline void Lock() {
        SocketLock.Lock();
    }

    inline void Unlock() {
        SocketLock.Unlock();
    }

    inline bool TryLock() {
        return SocketLock.TryLock();
    }

    SocketError SetSocketAddress(int SocketFD, SocketAddress &Address);

    SocketError SetNonBlock(bool On);

    SocketError SetNoDelay(bool On);

    static inline Socket *TimerToSocket(Timer *T) {
        if (T == nullptr) {
            return nullptr;
        }
        return (Socket *) ((uintptr_t) T - (uintptr_t) (&((Socket *) 0)->TimerNode));
    }

    virtual RuntimeError HandleEventDomain(EventType Type) = 0;

    virtual SocketError Close();
};
