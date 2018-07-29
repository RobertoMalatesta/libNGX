namespace ngx::Core {

    using namespace std;

    class Thread;

    typedef Promise *(PromiseCallback)(Promise *self, void *PointerToArg);

    class Promise : public Queue {
    private:
        void *PointerToArg = nullptr;
        ThreadPool *TPool = nullptr;
        PromiseCallback *Callback = nullptr;

        friend class ThreadPool;

    public:
        Promise(ThreadPool *TPool = nullptr);

        Promise(ThreadPool *TPool, PromiseCallback *Callback, void *PointerToArg);
        Promise(Thread *T, PromiseCallback *Callback, void *PointerToArgs);

        Promise *doPromise();
    };

    class Thread {

        private:

            static const uint PoolMemorySize = 409600;
            static const uint GCRound = 5000;

            thread WorkerThread;
            Pool *Allocator;
            atomic_flag Lock = ATOMIC_FLAG_INIT;
            bool Running = true;
            uint PostCount;
            Promise Sentinel;
            static void ThreadProcess(Thread *Thread);
            friend Promise;

        public:
            Thread();
            ~Thread();
            void Stop();
            int TryPostPromise(PromiseCallback *Callback, void * Argument);
    };

    class ThreadPool {

    private:
        int NumThread;
        int DeliverIndex = 0;
        vector<Thread *> Threads;

    public:
        ThreadPool(int NumThread = 8);
        ~ThreadPool();
        void PostPromise(PromiseCallback *Callback, void *PointerToArg);

    };
}