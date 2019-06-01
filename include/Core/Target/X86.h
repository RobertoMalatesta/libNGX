#ifndef LIBNGX_CORE_TARGET_X86
#define LIBNGX_CORE_TARGET_X86
namespace ngx {
namespace Core {
namespace Target {
namespace X86 {
extern "C" {
    void cpuinfo();
    void inline RelaxMachine() { asm volatile("rep; nop":: : "memory"); }
}
} // X86
} // Target
} // Core
} // ngx
#endif