namespace ngx::Core {

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
        SOCK_READ_WRITE_EVENT
    } SocketEventType;

    static void DiscardPromise(void *Argument, ThreadPool *TPool) {
        return;
    }

    class EventEntity {
        protected:
            PromiseCallback  *OnConnected = &DiscardPromise;
            PromiseCallback  *OnRead = &DiscardPromise;
            PromiseCallback  *OnWrite = &DiscardPromise;
            PromiseCallback  *OnClosed = &DiscardPromise;
            PromiseCallback  *OnEvent = &DiscardPromise;
    };

    union UserArgument {
        void *Ptr;
        unsigned int UInt;
    };

    struct EventPromiseArgs {
        UserArgument UserArguments[8];
    };

    class EventDomain {
        protected:
            Pool Allocator;
            ThreadPool TPool;
            TimerTree *Timers;
            SpinLock Lock;
        public:
            EventDomain(size_t PoolSize, int ThreadCount);
            ~EventDomain();
            static void DiscardPromise(void *Argument, ThreadPool *TPool) {};
            RuntimeError PostTimerEvent(uint32_t Seconds, PromiseCallback *Callback, void *Argument);
            RuntimeError PostPromise(PromiseCallback *Callback, void *Argument);
            RuntimeError EventDomainProcess(EventPromiseArgs *PointerToArgument);
    };
}