namespace ngx::Core {

    using namespace std;

    typedef Promise *(PromiseCallback)(Promise *self, void *PointerToArg);

    Promise *Sleep(Promise *, void *);

    class Promise : Queue {
        private:
            MemAllocator *Allocator = nullptr;
            PromiseCallback *Callback = Sleep;
            void *PointerToArg = nullptr;
            Promise(ThreadPool *TPool, PromiseCallback *Callback, void *PointerToArg);
            Promise () = default;
        public:
            static Promise SleepyPromise;

            static Promise *CreatePromise(ThreadPool *TPool, PromiseCallback *Callback , void *PointerToArg );
            void CleanSelf();
            Promise *doPromise();
    };

    class ThreadPool {

        private:
            int NumThread;
            Promise *Sentinel;
            atomic_flag PromiseQueueLock;
            MemAllocator * Allocator;
            vector<thread> Threads;
            void ThreadProcess();
            friend class Promise;
        public:
            void PostPromise(Promise *Next);
    };
}