namespace ngx {
    namespace Core {
        namespace DefaultConfig {

            const char CPUModule[] = "I386";
            const char Compiler[] = "Clang 6.0";

            const uint32_t ARRAY_DEFAULT_COUNT = 32;
            const int DEFAULT_POOL_RESIDUAL = 32;
            const uint64_t DEFAULT_RECYCLE_SIZE = 1024;

            const size_t BUILD_DEFAULT_SIZE = 4096;
            const size_t POOL_DEFAULT_BLOCK_SIZE = 40960;
            const size_t BUFFER_DEFAULT_BLOCK_SIZE = 40960;

            const int THREAD_WAIT_TIME = 1500;      // us
            const int THREAD_POOL_SPIN_TIME = 100;  // us
            const uint32_t THREAD_GC_ROUND = 20000;

            const bool LITTLE_ENDING = true;
            const bool NON_ALIGNED = true;
            const size_t PAGE_SIZE = 4096;
            static uint32_t CACHELINE_SIZE = 64;
            const uint64_t TIME_RESOLUTION = 1000;  // us
            const uint32_t NUM_TIME_SLOT = 128;
        }
    }
}
