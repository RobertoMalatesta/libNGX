
class SocketEventDomain : public EventDomain {
protected:
    inline EventType GetAttachedEvent(Socket &S) {
        return S.AttachedEvent;
    }

    inline void SetAttachedEvent(Socket &S, EventType Type, bool On) {

        if (On == 1) {
            S.AttachedEvent |= Type;
        } else {
            S.AttachedEvent &= ~Type;
        }
    }

    inline void LockSocket(Socket &S) {
        S.Lock.Lock();
    }

    inline void UnlockSocket(Socket &S) {
        S.Lock.Unlock();
    }

public:
    SocketEventDomain(size_t PoolSize, int ThreadCount);

    virtual EventError AttachSocket(Socket &S, EventType Type) = 0;

    virtual EventError DetachSocket(Socket &S, EventType Type) = 0;
};
