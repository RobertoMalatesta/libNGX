
class SocketEventDomain : public EventDomain {
protected:
    TimerHub THub;

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

    SocketEventDomain(int ThreadCount);

    virtual EventError AttachSocket(Socket &S, EventType Type) = 0;

    virtual EventError DetachSocket(Socket &S, EventType Type) = 0;

    inline RuntimeError SetTimer(Socket &S, uint64_t Interval, TimerMode Mode) {

        THub.DetachTimer(S.TimerNode);

        S.TimerNode.SeInterval(Interval, Mode);

        THub.AttachTimer(S.TimerNode);

        return {0};
    }

    inline RuntimeError ResetTimer(Socket &S) {
        THub.DetachTimer(S.TimerNode);
        return {0};
    }

    RuntimeError EventLoop();
};
