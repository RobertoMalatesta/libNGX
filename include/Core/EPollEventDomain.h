namespace ngx::Core {

    class EPollEventDomain : public EventDomain {
        private:
            int EPollFD = -1;

            static void EPollEventProcessPromise(void* Args, ThreadPool *TPoll);

        public:

            EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize);
            ~EPollEventDomain();

            EventError EPollAttachConnection(Connection *C);
            EventError EPollDetachConnection(Connection *C);
            EventError EPollPostListenPromise(Listening *Listening);


//            EventError EPoll

            RuntimeError EventDomainProcess();
    };
}