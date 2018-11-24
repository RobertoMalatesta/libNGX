enum TimerMode {
    TM_CLOSED = 0,
    TM_ONCE,
    TM_INTERVAL,
};

class Timer : public RBTreeNode, public CanReset {
protected:
    virtual int Compare(Timer *Node);
    uint64_t Timestamp = 0;
    friend class TimerTree;
public:
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

    virtual void Reset() { Timestamp = 0; };
};

class TimerTree : public RBTree {
private:
    Timer _Sentinet;
public:
    TimerTree();

    ~TimerTree();

    int QueueExpiredTimer(ThreadPool *TPool, uint32_t Count = TIMER_PRCESS_COUNT);

    int AttachTimer(Timer &T);

    int DetachTimer(Timer &T);
};

