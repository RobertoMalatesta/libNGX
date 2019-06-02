#include <cstdint>
#include <ctime>
#ifndef LIBNGX_CONFIG_HEADER
#define LIBNGX_CONFIG_HEADER

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

namespace ngx {
    namespace Core {
using namespace std;
        const size_t B = 1;
        const size_t KB = 1024 * B;
        const size_t MB = 1024 * KB;
        const size_t GB = 1024 * MB;

        const time_t  us = 1;
        const time_t  ms = 1000 * us;
        const time_t   s = 1000 * ms;
        const time_t min = 60 * s;
        const time_t   h = 60 * min;
        const time_t   d = 24 * h;

        namespace Config {
            // Architecture Configuration
            const char CPUModule[] = "I386";
            const char Compiler[] = "Clang 6.0";

            const bool LITTLE_ENDING = true;
            const bool NON_ALIGNED = true;

            const size_t PAGE_SIZE = 4 * KB;  // Byte(s)
            static uint32_t CACHELINE_SIZE = 64;    // Byte(s)
            const uint64_t TIME_RESOLUTION = 1000;  // us
            const uint32_t NUM_TIME_SLOT = 128;

            // ADT Configuration
            const uint32_t ARRAY_DEFAULT_ELEMENT_COUNT = 32;

            // Support Configuration
            const size_t BUFFER_MEMORY_BLOCK_SIZE = 128 * PAGE_SIZE; //Byte(s)
            const size_t POOL_MEMORY_BLOCK_SIZE = 8 * PAGE_SIZE;     //Byte(s)
            const uint64_t DEFAULT_COLLECTOR_SIZE = 1024;
            const int THREAD_WAIT_TIME = 1 * ms;                     // 1(ms)
            const int POOL_COLLECT_ROUND = 1000;
            const uint32_t THREAD_COLLECT_ROUND = 10000;
        }
    }
}
#endif