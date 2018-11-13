
typedef enum {
    ET_NONE = 0x0000,
    ET_TIMER = 0x0001,
    ET_CONNECTED = 0x0002,
    ET_READ = 0x0004,
    ET_WRITE = 0x0008,
    ET_CLOSED = 0x0010,
} EventType;

typedef enum {
    SOCK_READ_EVENT = 0,
    SOCK_WRITE_EVENT,
    SOCK_READ_WRITE_EVENT,
    SOCK_TIMER_EVENT,
} SocketEventType;


class EventDomain {
protected:
    Pool Allocator;
    ThreadPool TPool;
    TimerTree Timers;
    SpinLock Lock;
public:
    EventDomain(size_t PoolSize, int ThreadCount);

    ~EventDomain() = default;

    static void DiscardPromise(void *Argument, ThreadPool *TPool) {};

    inline RuntimeError AttachTimer(Timer &Timer) {
        Timers.AttachTimer(Timer);
        return {0};
    }

    inline RuntimeError DetachTimer(Timer &Timer) {
        Timers.DetachTimer(Timer);
        return {0};
    }

    RuntimeError PostPromise(PromiseCallback *Callback, void *Argument);

    virtual RuntimeError EventDomainProcess(EventPromiseArgs &Argument);
};
