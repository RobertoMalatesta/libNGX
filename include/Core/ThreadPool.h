namespace ngx::Core {

    using namespace std;

    class Thread;
    class ThreadPool;

    typedef void (PromiseCallback)(void *PointerToArguments, ThreadPool *TPool);

    class Promise : public Queue {
    private:
        void *PointerToArg = nullptr;
        ThreadPool *TPool = nullptr;
        PromiseCallback *Callback = nullptr;

        friend class Thread;
        friend class ThreadPool;

    public:
        Promise();

        Promise(ThreadPool *TPool, Thread *T, PromiseCallback *Callback, void *PointerToArg);
        Promise(Thread *T, PromiseCallback *Callback, void *PointerToArgs);

        void doPromise();
    };

    class Thread {

        private:

            static const uint PoolMemorySize = 409600;
            static const uint GCRound = 5000;

            ThreadPool *TPool = nullptr;
            thread WorkerThread;
            Pool *Allocator = nullptr;
            atomic_flag Lock = ATOMIC_FLAG_INIT;
            bool Running = true;
            uint PostCount;
            Promise Sentinel;
            static void ThreadProcess(Thread *Thread);
            friend Promise;

        public:
            Thread(ThreadPool *TPool);
            ~Thread();
            void Stop();
            int TryPostPromise(PromiseCallback *Callback, void * Argument);
    };

    class ThreadPool {

    private:
        int NumThread;
        int DeliverIndex = 0;
        vector<Thread *> Threads;
        friend class Promise;

    public:
        ThreadPool(int NumThread = 8);
        ~ThreadPool();
        void PostPromise(PromiseCallback *Callback, void *PointerToArg);

    };
}