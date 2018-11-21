
class SocketEventDomain : public EventDomain {
protected:
    TimerTree Timers;

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

public:
    SocketEventDomain(size_t PoolSize, int ThreadCount);
    ~SocketEventDomain();

    virtual EventError AttachSocket(Socket &S, EventType Type) = 0;

    virtual EventError DetachSocket(Socket &S, EventType Type) = 0;

    inline RuntimeError SetTimer(Socket &S, uint64_t Interval, TimerMode Mode) {

        LockGuard LockGuard(&Lock);
        S.Lock();

        Timers.DetachTimer(S.TimerNode);
        S.TimerNode.SeInterval(Interval, Mode);
        Timers.AttachTimer(S.TimerNode);

        S.Unlock();
        return {0};
    }

    inline RuntimeError ResetTimer(Socket &S) {

        LockGuard LockGuard(&Lock);
        S.Lock();

        Timers.DetachTimer(S.TimerNode);

        S.Unlock();
        return {0};
    }

    RuntimeError EventDomainProcess();
};
