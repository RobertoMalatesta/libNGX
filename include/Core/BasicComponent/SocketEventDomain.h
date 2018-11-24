
class SocketEventDomain : public EventDomain {
protected:
    TimerTree Timers;
    SpinLock TimersLock;

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

        TimersLock.Lock();

        Timers.DetachTimer(S.TimerNode);
        S.TimerNode.SeInterval(Interval, Mode);
        Timers.AttachTimer(S.TimerNode);

        TimersLock.Unlock();

        return {0};
    }

    inline RuntimeError ResetTimer(Socket &S) {

        TimersLock.Lock();

        Timers.DetachTimer(S.TimerNode);

        TimersLock.Unlock();

        return {0};
    }

    RuntimeError EventDomainProcess();
};
