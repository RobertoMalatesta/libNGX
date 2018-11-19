enum TimerMode {
    TM_CLOSED = 0,
    TM_ONCE,
    TM_INTERVAL,
};

class Timer : public RBTreeNode, public CanReset {
protected:
    virtual int Compare(Timer *Node);

    friend class TimerTree;

    Spinlock Lock;
    uint64_t Timestamp = 0;
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

class TimerTree : public RBTree, public AllocatorBuild<Timer> {
public:
    TimerTree(MemAllocator *Allocator);

    ~TimerTree();

    int QueueExpiredTimer(ThreadPool *TPool);

    int AttachTimer(Timer &T);

    int DetachTimer(Timer &T);
};

