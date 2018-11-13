class Timer : public RBTreeNode, public CanReset {

    friend class TimerTree;

protected:
    bool On = false;
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

    virtual void Reset() {On = false;};
};

class TimerTree : public RBTree, public AllocatorBuild<Timer> {
    SpinLock Lock;
public:
    TimerTree(MemAllocator *Allocator);

    ~TimerTree();

    int QueueExpiredTimer(ThreadPool *TPool);

    int AttachTimer(Timer &T);
};

