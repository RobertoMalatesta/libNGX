
class SocketEventDomain : public EventDomain {
protected:
    inline EventType GetAttachedEvent(Socket &S) {
        SpinlockGuard LockGuard(&S.Lock);

        return S.AttachedEvent;
    }

    inline void SetAttachedEvent(Socket &S, EventType Type, bool On) {
        SpinlockGuard LockGuard(&S.Lock);

        if (On == 1) {
            S.AttachedEvent |= Type;
        } else {
            S.AttachedEvent &= ~Type;
        }
    }

public:
    SocketEventDomain(size_t PoolSize, int ThreadCount);

    virtual EventError AttachSocket(Socket &S, EventType Type) = 0;

    virtual EventError DetachSocket(Socket &S, EventType Type) = 0;
};
