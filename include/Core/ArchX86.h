namespace ngx::Core {

    extern "C" {
    inline void cpuid(uint32_t i, uint32_t *buf);
    void cpuinfo();
    void inline RelaxMachine() { asm volatile("rep; nop":: : "memory"); }
    }
}