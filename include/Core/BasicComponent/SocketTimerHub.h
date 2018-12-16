enum TimerMode {
    TM_CLOSED = 0,
    TM_ONCE,
    TM_INTERVAL,
};

class Timer: public RBNode, public CanReset {
public:
    uint64_t Timestamp = 0;
    TimerMode Mode = TM_ONCE;
    uint64_t Interval = 0;
    Job TimerJob;

    Timer() = default;

    Timer(uint64_t Timestamp, ThreadFn *Callback, void *Argument): TimerJob(Callback, Argument) {
        this->Timestamp = Timestamp;
    };

    ~Timer() = default;

    void SeInterval(uint64_t Interval, TimerMode Mode = TM_ONCE) {
        this->Mode = Mode, this->Interval = Interval;
    }

    inline bool IsTimerAttached() {
        return GetLeft() != nullptr && GetRight() != nullptr;
    }

    uint64_t GetTimestamp() { return Timestamp; }

    virtual int operator - (RBNode &R) {
        return Timestamp >  ((Timer &)R).Timestamp ? 1 : -1;
    }

    virtual int operator - (uint64_t R) {
        return this->Timestamp > R? 1 : -1;
    }

    virtual void Reset() { Timestamp = 0; };
};

class SocketTimerHub : public RBT {
private:
    SpinLock TimerHubLock;
public:
    SocketTimerHub() = default;

    ~SocketTimerHub();

    int QueueExpiredTimer();

    int AttachTimer(Timer &T);

    int DetachTimer(Timer &T);
};

