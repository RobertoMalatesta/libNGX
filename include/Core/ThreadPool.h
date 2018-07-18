namespace ngx::Core {

    using namespace std;

    typedef Promise *(PromiseCallback)(Promise *self, void *PointerToArg);

    Promise *Sleep(Promise *, void *);

    class Promise : Queue {
        private:
            void *PointerToArg = nullptr;
            MemAllocator *Allocator = nullptr;
            PromiseCallback *Callback = Sleep;

            Promise(ThreadPool *TPool, PromiseCallback *Callback, void *PointerToArg);
            Promise () = default;
        public:
            static Promise SleepyPromise;
            static int PostPromise(ThreadPool *TPool, PromiseCallback *Callback , void *PointerToArg );
            void CleanSelf();
            Promise *doPromise();
    };

    class ThreadPool {

        private:
            int NumThread;
            Promise *Sentinel;
            atomic_flag PromiseQueueLock = ATOMIC_FLAG_INIT;
            MemAllocator * Allocator;
            vector<thread> Threads;
            void ThreadProcess();
            friend class Promise;

        public:


    };
}