namespace X86 {
    extern "C" {

    void cpuinfo();

    void inline RelaxMachine() { asm volatile("rep; nop":: : "memory"); }
    }
}
