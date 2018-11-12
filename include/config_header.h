namespace ngx{
    namespace Core {
        namespace DefaultConfig {

            const char CPUModule [] = "I386";
            const char Compiler[] = "Clang 6.0";

            const uint32_t ARRAY_DEFAULT_COUNT = 32;
            const int EPOLL_EVENT_BATCH_SIZE = 512;
            const int EPOLL_EVENT_WAIT_TIME = 10;
            const int EPOLL_EVENT_MAX_CONNECTION = 32768;
            const int THREAD_WAIT_TIME = 10000;
            const int THREAD_POOL_SPIN_TIME = 500;
            const uint64_t DEFAULT_RECYCLE_SIZE = 1024;
            const size_t BUFFER_DEFAULT_BLOCK_SIZE = 4096;
            const uint THREAD_POOL_MEMORY_SIZE = 409600;
            const uint THREAD_GC_ROUND = 5000;
            const int DEFAULT_POOL_RESIDUAL = 8;

            const bool LittleEnding = true;
            const bool NonAligned = true;
            static size_t PageSize = 4096;
            static uint32_t CacheLineSize = 64;
            const uint64_t TimeResolution = 1000;
            const uint32_t NumTimeSlot = 128;
        }
    }
}
