namespace ngx::Core {

    const int EPOLL_EVENT_BATCH_SIZE = 512;
    const int EPOLL_EVENT_WAIT_TIME = 10;
    const int EPOLL_EVENT_MAX_CONNECTION = 32768;

    struct EPollEventDomainArgument: SocketEventDomainArgument {
        epoll_event *Events;
        int EventCount;
    };

    class EPollEventDomain : public SocketEventDomain, MemAllocator{
        protected:
            int EPollFD = -1;
            atomic_flag Waiting = ATOMIC_FLAG_INIT;
            PromiseCallback *EventPromise;
        public:

            EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize, PromiseCallback  *ProcessPromise);
            ~EPollEventDomain();

            EventError AttachSocket(Socket *S, SocketEventType Type);
            EventError DetachSocket(Socket *S, SocketEventType Type);
            RuntimeError EventDomainProcess(void *PointerToArgument);

            void *Allocate(size_t Size);
            void Free(void **Pointer);
            void GC();
    };
}