namespace ngx::Core {

    using namespace std;

    typedef Promise *(PromiseCallback)(Promise *self, void *PointerToArg);

    Promise *Sleep(Promise *, void *);

    extern Promise SleepyPromise;

    class Promise : public Queue {
        private:
            void *PointerToArg = nullptr;
            MemAllocator *Allocator = nullptr;
            PromiseCallback *Callback = Sleep;
        public:
            Promise () = default;
            Promise(ThreadPool *TPool, PromiseCallback *Callback, void *PointerToArg);
            static int PostPromise(ThreadPool *TPool, PromiseCallback *Callback , void *PointerToArg );
            void CleanSelf();
            Promise *doPromise();
    };

    class ThreadPool {

        private:
            bool Running;
            int NumThread = 8;
            Promise *Sentinel;
            atomic_flag PromiseQueueLock = ATOMIC_FLAG_INIT;
            MemAllocator * Allocator;
            vector<thread *> Threads;
            static void ThreadProcess(ThreadPool *Pool);
            friend class Promise;

        public:
            void Start();
            void Stop();
    };
}