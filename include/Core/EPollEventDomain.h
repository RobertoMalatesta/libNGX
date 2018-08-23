namespace ngx::Core {

    class EPollEventDomain : public EventDomain {
        private:
            int EPollFD = -1;

        public:

            EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize);
            ~EPollEventDomain();

            int EPollAdd();
            int EPollRemove();
            int EPollModify();

            int EPollWaitEvent();
    };
}