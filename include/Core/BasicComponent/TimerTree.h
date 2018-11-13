class Timer : public RBTreeNode {

    friend class TimerTree;

protected:
    uint64_t Timestamp = 0;
    PromiseCallback *Callback = nullptr;
    void *Argument = nullptr;

    virtual int Compare(Timer *Node);

public:

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
};

class TimerTree : public RBTree, public AllocatorBuild<Timer> {
public:
    TimerTree(MemAllocator *Allocator);

    ~TimerTree();

    int PostTimerPromise(uint64_t Seconds, PromiseCallback *Callback, void *Argument);

    int QueueExpiredTimer(ThreadPool *TPool);

    int AttachTimer(Timer &T);
    int DetachTimer(Timer &T);
};

