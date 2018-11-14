class Timer : public RBTreeNode, public CanReset {

    friend class TimerTree;

protected:
    virtual int Compare(Timer *Node);

public:

    uint64_t Timestamp = 0;
    void *Argument = nullptr;
    PromiseCallback *Callback = nullptr;

    Timer() {};

    Timer(uint64_t Timestamp, PromiseCallback *Callback, void *Argument) {
        this->Timestamp = Timestamp;
        this->Callback = Callback;
        this->Argument = Argument;
    };

    ~Timer() = default;

    void SetExpireTime(uint64_t Timestamp) {
        this->Timestamp = Timestamp;
    }

    inline bool IsTimerAttached() {
        return GetLeft() != nullptr && GetRight() != nullptr;
    }

    virtual void Reset() {Timestamp = 0;};
};

class TimerTree : public RBTree, public AllocatorBuild<Timer> {
public:
    TimerTree(MemAllocator *Allocator);

    ~TimerTree();

    int QueueExpiredTimer(ThreadPool *TPool);

    int AttachTimer(Timer &T);

    int DetachTimer(Timer &T);
};

