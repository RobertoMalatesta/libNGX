namespace ngx::Core {

    using namespace std;

    typedef Promise *(PromiseCallback)(Promise *self, void *PointerToArg);

    Promise *Sleep(Promise *, void *);

    extern Promise SleepyPromise;

    class Promise : public Queue {
        private:
            void *PointerToArg = nullptr;
            ThreadPool *TPool = nullptr;
            PromiseCallback *Callback = Sleep;
            friend class ThreadPool;
        public:
            Promise (ThreadPool *TPool = nullptr);
            Promise(ThreadPool *TPool, PromiseCallback *Callback, void *PointerToArg);
            Promise *doPromise();
    };

    class ThreadPool {

        private:
            bool Running = false;
            int NumThread = 8;
            Promise Sentinel;
            atomic_flag PromiseQueueLock = ATOMIC_FLAG_INIT;
            MemAllocator * Allocator;
            vector<thread *> Threads;
            static void ThreadProcess(ThreadPool *Pool);
            friend class Promise;
        public:
            ThreadPool(MemAllocator *Allocator, int NumThread);
            void Start();
            void Stop();
            int PostPromise(PromiseCallback *Callback , void *PointerToArg );
            inline void Lock() {
                while (PromiseQueueLock.test_and_set()) {
                    RelaxMachine();
                }
            };
            inline void Unlock() {
                PromiseQueueLock.clear();
            }
    };
}