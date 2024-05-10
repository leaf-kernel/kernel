#include <arch/x86_64/acpi/fadt.h>
#include <arch/x86_64/cpu/utils.h>
#include <drivers/serial/serial.h>

void disable_interrupts() { __asm__ volatile("cli" : : : "memory"); }

void enable_interrupts() { __asm__ volatile("sti" : : : "memory"); }

void _shutdown_emu() {
    outw(0xB004, 0x2000);
    outw(0x604, 0x2000);
    outw(0x4004, 0x3400);
    outw(0x600, 0x34);
}

void _legacy_reboot() {
#define KBRD_INTRFC 0x64

#define KBRD_BIT_KDATA 0
#define KBRD_BIT_UDATA 1

#define KBRD_IO 0x60
#define KBRD_RESET 0xFE

#define bit(n) (1 << (n))

#define check_flag(flags, n) ((flags)&bit(n))

    uint8_t temp;

    asm volatile("cli");
    do {
        temp = inb(KBRD_INTRFC);
        if(check_flag(temp, KBRD_BIT_KDATA) != 0)
            inb(KBRD_IO);
    } while(check_flag(temp, KBRD_BIT_UDATA) != 0);

    outb(KBRD_INTRFC, KBRD_RESET);

loop:
    asm volatile("hlt");
    goto loop;
}

void _reboot() {
    if(_acpi_mode) {
        outb(fadt_table->ResetReg.Address, fadt_table->ResetValue);
    }

    _legacy_reboot();
}