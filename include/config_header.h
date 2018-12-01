namespace ngx {
    namespace Core {
        namespace DefaultConfig {

            // Architecture Configuration
            const char CPUModule[] = "I386";
            const char Compiler[] = "Clang 6.0";
            const bool LITTLE_ENDING = true;
            const bool NON_ALIGNED = true;
            const size_t PAGE_SIZE = 4096;          // Byte(s)
            static uint32_t CACHELINE_SIZE = 64;    // Byte(s)
            const uint64_t TIME_RESOLUTION = 1000;  // us
            const uint32_t NUM_TIME_SLOT = 128;

            // Types Configuration
            const size_t BUILD_DEFAULT_SIZE = 4096; // Byte(s)
            const uint32_t ARRAY_DEFAULT_ELEMENT_COUNT = 32;

            // BasicComponent Configuration
            const u_char LG2_MEMORY_BLOCK_SIZE_MIN = 12;          // 4KB(ytes)
            const u_char LG2_MEMORY_BLOCK_SIZE_MAX = 24;          // 16MB(ytes)
            const u_char LG2_BUFFER_MEMORY_BLOCK_SIZE = 16;       // 256KB(ytes)
            const u_char LG2_POOL_MEMORY_BLOCK_SIZE = 16;         // 256KB(ytes)
            const size_t MEMORY_BLOCK_SIZE_MIN = 4096;            // 4KB(ytes)
            const size_t MEMORY_BLOCK_SIZE_MAX = 1 * 1024 * 1024; // 10MB(ytes)
            const size_t BUFFER_MEMORY_BLOCK_SIZE = 1ULL << LG2_BUFFER_MEMORY_BLOCK_SIZE; // 256KB(ytes)
            const size_t POOL_MEMORY_BLOCK_SIZE = 1ULL << LG2_POOL_MEMORY_BLOCK_SIZE;     // 256KB(ytes)
            const int POOL_RESIDUAL = 16;
            const int POOL_RECYCLE_ROUND = 10000;
            const uint64_t DEFAULT_RECYCLE_SIZE = 1024;
            const int THREAD_WAIT_TIME =  100 * 1000;     // ns
            const int THREAD_POOL_SPIN_TIME = 40 * 1000;  // ns
            const uint32_t THREAD_GC_ROUND = 10000;
            const uint32_t RECYCLE_GC_ROUND = 10000;
            const uint32_t TIMER_PROCESS_COUNT = 500;
        }
    }
}
