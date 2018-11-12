namespace X86_64 {
    extern "C" {
    void cpuinfo();
    void inline RelaxMachine() { asm volatile("rep; nop":: : "memory"); }
    }
}
