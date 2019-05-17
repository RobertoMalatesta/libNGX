namespace ngx {
    namespace Core {
        namespace Config {

            // Architecture Configuration
            const char CPUModule[] = "I386";
            const char Compiler[] = "Clang 6.0";

            const bool LITTLE_ENDING = true;
            const bool NON_ALIGNED = true;

            const size_t PAGE_SIZE = 4096;  // Byte(s)
            static uint32_t CACHELINE_SIZE = 64;    // Byte(s)
            const uint64_t TIME_RESOLUTION = 1000;  // us
            const uint32_t NUM_TIME_SLOT = 128;

            // ADT Configuration
            const uint32_t ARRAY_DEFAULT_ELEMENT_COUNT = 32;

            // Support Configuration
            const size_t BUFFER_MEMORY_BLOCK_SIZE = 128 * PAGE_SIZE;    //Byte(s)
            const size_t POOL_MEMORY_BLOCK_SIZE = 128 * PAGE_SIZE;      //Byte(s)
            const int POOL_COLLECT_ROUND = 1000;
            const uint64_t DEFAULT_COLLECTOR_SIZE = 1024;
            const int THREAD_WAIT_TIME = 1 * 1000 * 1000;     // 1(ms)
            const uint32_t THREAD_COLLECT_ROUND = 10000;
        }
    }
}
