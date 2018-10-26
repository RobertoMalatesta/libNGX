namespace ngx{
    namespace Core {
        namespace DefaultConfig {
            const char CPU_Module[] = "AMD64";
            const char Compiler[] = "Clang 6.0";
            const int EPOLL_EVENT_BATCH_SIZE = 512;
            const int EPOLL_EVENT_WAIT_TIME = 5;
            const int EPOLL_EVENT_MAX_CONNECTION = 32768;
            const int THREAD_WAIT_TIME = 1000;
            const int THREAD_POOL_SPIN_TIME = 50;
            const uint64_t DEFAULT_RECYCLE_SIZE = 1024;
            const size_t BUFFER_DEFAULT_BLOCK_SIZE = 4096;
            const uint THREAD_POOL_MEMORY_SIZE = 409600;
            const uint THREAD_GC_ROUND = 5000;
            const int DEFAULT_POOL_RESIDUAL = 8;
        }
    }
}
