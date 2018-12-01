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
    void *Argument = nullptr;
    PromiseCallback *Callback = nullptr;

    Timer() {};

    Timer(uint64_t Timestamp, PromiseCallback *Callback, void *Argument) {
        this->Timestamp = Timestamp;
        this->Callback = Callback;
        this->Argument = Argument;
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

class TimerHub : public RBT {
private:
    SpinLock TimerHubLock;
public:
    TimerHub() = default;

    ~TimerHub();

    int QueueExpiredTimer(ThreadPool *TPool, uint32_t Count = TIMER_PROCESS_COUNT);

    int AttachTimer(Timer &T);

    int DetachTimer(Timer &T);
};

