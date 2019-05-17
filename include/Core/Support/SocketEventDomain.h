
class SocketEventDomain : public EventDomain {
protected:
    SocketTimerTree THub;

public:

    SocketEventDomain(int ThreadCount);

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
