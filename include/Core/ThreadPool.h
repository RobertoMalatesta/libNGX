namespace ngx::Core {

    using namespace std;


    class Promise {

    };

    class ThreadPool {

    private:
        MemAllocator * Allocator;
        int NumThread;
        vector<thread> Threads;

    };
}