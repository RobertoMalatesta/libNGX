#include <Core/Core.h>
#include <unistd.h>

namespace ngx::Core {

    Promise::Promise(ThreadPool *Pool) : Queue() {
        this->TPool = Pool;
    }

    Promise::Promise(Thread *T, PromiseCallback *Callback, void *PointerToArgs)
            : Queue((Queue *) &T->Sentinel, false) {
        this->Callback = Callback;
        this->PointerToArg = PointerToArgs;
    }

    Promise *Promise::doPromise() {

        Promise *Next = nullptr;

        if (Callback != nullptr) {
            Next = Callback(this, PointerToArg);
        }

        return Next;
    }

    Thread::Thread() :Sentinel(), WorkerThread(Thread::ThreadProcess, this){

        Allocator = new Pool(PoolMemorySize);
        Lock.clear();
    }

    Thread::~Thread() {
        Stop();
        delete Allocator;
    }

    int Thread::TryPostPromise( PromiseCallback *Callback, void *Argument){

        if (Lock.test_and_set()) {
            return -1;
        }

        if (!Running) {
            return  0;
        }

        void *PointerToPromise = Allocator->Allocate(sizeof(Promise));

        if (nullptr == PointerToPromise) {
            Lock.clear();
            return -1;
        }

        new(PointerToPromise) Promise(this, Callback, Argument);

        if (PostCount++ % 1000 == 0) {
            Allocator->GC();
        }

        Lock.clear();

        return 0;
    }

    void Thread::ThreadProcess(ngx::Core::Thread *Thread) {

        Promise *Node;
        bool IsRunnig = Thread->Running;

        while (IsRunnig) {

            usleep(1000);

            while (Thread->Lock.test_and_set()) {
                RelaxMachine();
            }

            while(!Thread->Sentinel.IsEmpty()) {

                Node = (Promise *) Thread->Sentinel.GetHead();
                Node->Detach();
                Node->doPromise();
                Thread->Allocator->Free((void **)&Node);
            }

            IsRunnig = Thread->Running;
            Thread->Lock.clear();
        }
    }

    void Thread::Stop() {

        while (Lock.test_and_set()) {
            RelaxMachine();
        }
        Running = false;
        Lock.clear();
        WorkerThread.join();
        Allocator->GC();
    }

    ThreadPool::ThreadPool(int NumThread) {
        this->NumThread = NumThread;

        while(NumThread-- > 0) {
            Threads.push_back(new Thread());
        }
    }

    ThreadPool::~ThreadPool() {
        for (Thread *Temp: Threads) {
            delete Temp;
        }
        Threads.clear();
    }

    void ThreadPool::PostPromise(PromiseCallback *Callback, void *PointerToArg) {

        int RetCode, i=DeliverIndex;

        do {
            RetCode = Threads[(i) % NumThread]->TryPostPromise(Callback, PointerToArg);

            if (RetCode == 0) {
                break;
            }

            if (((i -= RetCode)-DeliverIndex) % NumThread == 0) {
                usleep(10);
            }
        } while (true);

        DeliverIndex = i;
    }
}