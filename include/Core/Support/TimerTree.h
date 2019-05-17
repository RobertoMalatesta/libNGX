enum TimerMode {
    TM_CLOSED = 0,
    TM_ONCE,
    TM_INTERVAL,
};

class Timer : public RBNode {
public:
    uint64_t Timestamp = 0;
    TimerMode Mode;
    uint64_t Interval;
    Job TimerJob;

    Timer() = default;
    Timer(uint64_t Timestamp, ThreadFn *Callback, void *Argument) :
            Timestamp(Timestamp),
            Mode(TM_CLOSED),
            Interval(0),
            TimerJob(Callback, Argument) {
    };

    ~Timer() = default;

    void SeInterval(uint64_t Interval, TimerMode Mode = TM_ONCE) {
        this->Mode = Mode, this->Interval = Interval;
    }

    inline bool IsTimerAttached() {
        return getParent() != this;
    }

    uint64_t GetTimestamp() { return Timestamp; }

    virtual int operator-(RBNode &R) {
        return Timestamp > ((Timer &) R).Timestamp ? 1 : -1;
    }

    virtual int operator-(uint64_t R) {
        return this->Timestamp > R ? 1 : -1;
    }

    virtual void Reset() { Timestamp = 0; };
};

class TimerTree : protected RBTree {
private:
    spin_lock TimerHubLock;
public:
    TimerTree() = default;
    ~TimerTree();

    RuntimeError attachTimer(Timer &T);
    RuntimeError detachTimer(Timer &T);
    RuntimeError preemptTimer(Timer &T);

    Timer *begin() const;
    Timer *next(Timer*) const;

    RuntimeError nextExpiredTimer(uint64_t Timestamp, Timer *&T);
};

