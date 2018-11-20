namespace ngx {
    namespace Core {
        namespace DefaultConfig {

            // Architecture Configuration
            const char CPUModule[] = "I386";
            const char Compiler[] = "Clang 6.0";
            const bool LITTLE_ENDING = true;
            const bool NON_ALIGNED = true;
            const size_t PAGE_SIZE = 4096;  // Byte(s)
            static uint32_t CACHELINE_SIZE = 64;    // Byte(s)
            const uint64_t TIME_RESOLUTION = 1000;  // us
            const uint32_t NUM_TIME_SLOT = 128;

            // DataStructure Configuration
            const size_t BUILD_DEFAULT_SIZE = 4096; // Byte(s)
            const uint32_t ARRAY_DEFAULT_ELEMENT_COUNT = 32;

            // BasicComponent Configuration
            const size_t BUFFER_MEMORY_BLOCK_SIZE = 1 * 128 * PAGE_SIZE;    //Byte(s)
            const size_t POOL_MEMORY_BLOCK_SIZE = 1 * 128 * PAGE_SIZE;      //Byte(s)
            const int POOL_RESIDUAL = 16;
            const int POOL_RECYCLE_ROUND = 10000;
            const uint64_t DEFAULT_RECYCLE_SIZE = 1024;
            const int THREAD_WAIT_TIME = 1000;     // us
            const int THREAD_POOL_SPIN_TIME = 20;  // us
            const uint32_t THREAD_GC_ROUND = 10000;
            const uint32_t RECYCLE_GC_ROUND = 10000;
        }
    }
}
