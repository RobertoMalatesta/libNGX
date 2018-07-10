#include "Core/Core.h"

namespace ngx::Core {

    using namespace Config;
    inline void cpuid(uint32_t i, uint32_t *buf) {

        if (strcmp(CPU_Module,"I386") == 0) {
            __asm__ (

            "    mov    %%ebx, %%esi;  "

                    "    cpuid;                "
                    "    mov    %%eax, (%1);   "
                    "    mov    %%ebx, 4(%1);  "
                    "    mov    %%edx, 8(%1);  "
                    "    mov    %%ecx, 12(%1); "

                    "    mov    %%esi, %%ebx;  "

            : : "a" (i), "D" (buf) : "ecx", "edx", "esi", "memory" );
        }
        else {
            uint32_t  eax, ebx, ecx, edx;

            __asm__ (

            "cpuid"

            : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) : "a" (i) );

            buf[0] = eax;
            buf[1] = ebx;
            buf[2] = edx;
            buf[3] = ecx;
        }
    };

    void cpuinfo() {

        u_char *vendor;
        uint32_t vbuf[5], cpu[4], model;

        if (strcmp(CPU_Module, "I386") == 0 || strcmp(CPU_Module, "AMD64") == 0 ||
            strcmp(CPU_Module, "GCC") == 0 || strcmp(CPU_Module, "INTEL") == 0) {
            return;
        }

        vbuf[0] = 0;
        vbuf[1] = 0;
        vbuf[2] = 0;
        vbuf[3] = 0;
        vbuf[4] = 0;

        cpuid(0, vbuf);

        vendor = (u_char *) &vbuf[1];

        if (vbuf[0] == 0) {
            return;
        }

        cpuid(1, cpu);

        if (strcmp((const char *) vendor, "GenuineIntel") == 0) {

            switch ((cpu[0] & 0xf00) >> 8) {

                /* Pentium */
                case 5:
                    CacheLineSize = 32;
                    break;

                    /* Pentium Pro, II, III */
                case 6:
                    CacheLineSize = 32;

                    model = ((cpu[0] & 0xf0000) >> 8) | (cpu[0] & 0xf0);

                    if (model >= 0xd0) {
                        /* Intel Core, Core 2, Atom */
                        CacheLineSize = 64;
                    }

                    break;

                    /*
                     * Pentium 4, although its cache line size is 64 bytes,
                     * it prefetches up to two cache lines during memory read
                     */
                case 15:
                    CacheLineSize = 128;
                    break;
            }

        } else if (strcmp((const char *) vendor, "AuthenticAMD") == 0) {
            CacheLineSize = 64;
        }
    }
}