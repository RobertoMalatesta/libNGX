
class Server {

protected:
    SpinLock Lock;
    SocketEventDomain *EventDomain;

    virtual RuntimeError OnLoopFinished() = 0;

public:
    Server(SocketEventDomain *BackendEventDomain): EventDomain(BackendEventDomain), Lock() {};

    // Server Main EventLoop
    virtual RuntimeError ServerEventLoop() = 0;

    // Set Server EventDomain if not
    RuntimeError SetEventDomain(SocketEventDomain *BackendEventDomain) {

        if (EventDomain != nullptr) {
            return {EALREADY, "EventDomain already set"};
        }

        EventDomain = BackendEventDomain;
        return {0};
    }
};
