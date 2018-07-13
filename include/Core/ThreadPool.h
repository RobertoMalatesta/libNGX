namespace ngx::Core {

    using namespace std;

    typedef Promise *(PromiseCallback)(Promise *self, void *PointerToArg);

    class Promise {

        private:
            ThreadPool *TPool;
            MemAllocator *Allocator;
            PromiseCallback *Callback;
            void *PointerToArg;


        public:
            Promise();
            static Promise *CreatePromise(ThreadPool *Pool, PromiseCallback *Callback, void *PointerToArg, MemAllocator *Allocator= nullptr);
            void CleanSelf();
            void doPromise();
            static const Promise *GetIdlePromise();
    };

    class ThreadPool {

        private:
            MemAllocator * Allocator;

            int NumThread;
            Queue *Sentinel;

            vector<thread> Threads;

            void ThreadProcess();
        public:
            void PostPromise(Promise *Next);
    };
}