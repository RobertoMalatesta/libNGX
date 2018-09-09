namespace ngx::Core {

    class EPollEventDomain : public EventDomain {
        private:
            int EPollFD = -1;

        public:

            EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize);
            ~EPollEventDomain();

            EventError EPollAttachConnection(Connection *C);
            EventError EPollDetachConnection(Connection *C);

//            EventError EPoll

            RuntimeError EventDomainProcess();
    };
}