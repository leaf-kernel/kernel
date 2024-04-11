#include <arch/cpu/cpu.h>
#include <drivers/stdio/printf.h>

void hcf()
{
    asm("cli");
    hlt();
}

void hlt()
{
    for (;;)
    {
        asm("hlt");
    }
}

void panic(const char *reason, int_frame_t frame)
{
    // TODO: Get the CPU ID
    int cpuId = 1;
    dprintf("panic(cpu %d @ 0x%016llx): type %d (%s)!\n", cpuId, frame.rip, frame.vector, reason);
    dprintf("rax: 0x%.16llx, rbx: 0x%.16llx, rcx: 0x%.16llx, rdx: 0x%.16llx\n",
            frame.rax, frame.rbx, frame.rcx, frame.rdx);

    dprintf("rsp: 0x%.16llx, rbp: 0x%.16llx, rsi: 0x%.16llx, rdi: 0x%.16llx\n",
            frame.rsp, frame.rbp, frame.rsi, frame.rdi);

    dprintf("r8:  0x%.16llx, r9:  0x%.16llx, r10: 0x%.16llx, r11: 0x%.16llx\n",
            frame.r8, frame.r9, frame.r10, frame.r11);

    dprintf("r12: 0x%.16llx, r13: 0x%.16llx, r14: 0x%.16llx, r15: 0x%.16llx\n",
            frame.r12, frame.r13, frame.r14, frame.r15);

    dprintf("rfl: 0x%.16llx, rip: 0x%.16llx, cs:  0x%.16llx, ss:  0x%.16llx\n",
            frame.rflags, frame.rip, frame.cs, frame.ss);
    hcf();
}