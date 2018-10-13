namespace ngx::Core {

    /*
     * EPollArguments
     *
     * UserArgument[0] = temp arg0
     * UserArgument[1] = temp arg1
     * UserArgument[2] = temp arg2
     * UserArgument[3] = Server
     * UserArgument[4] = EventDomain
     * UserArgument[5] = Listening
     * UserArgument[6] = Connection
     * UserArgument[7] = EventType
     *
     * */

    class EPollEventDomain : public SocketEventDomain, MemAllocator{
        protected:
            int EPollFD = -1;
            atomic_flag Waiting = ATOMIC_FLAG_INIT;
        public:

            EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize);
            ~EPollEventDomain();

            EventError AttachSocket(Socket *S, SocketEventType Type);
            EventError DetachSocket(Socket *S, SocketEventType Type);
            RuntimeError EventDomainProcess(EventPromiseArgs *PointerToArgument);

            void *Allocate(size_t Size);
            void Free(void **Pointer);
            void GC();
    };
}