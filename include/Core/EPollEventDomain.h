namespace ngx::Core {
    struct EPollEventDomainArgument {
        EPollEventDomain *EventDomain;
        class Listening *Listening;
        epoll_event *Events;
        int EventCount;
        UserArgument UserArguments[4];
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