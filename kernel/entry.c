// Standard libgcc includes
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Bootloader imports
#include <limine/limine.h>

// Arch specific imports
#include <arch/cpu/cpu.h>
#include <arch/pit/pit.h>
#include <arch/idt/idt.h>

// Memory imports
#include <memory/pmm.h>
#include <memory/kheap.h>

// Logging imports
#include <sys/logger.h>
#include <drivers/stdio/printf.h>
#include <drivers/tty/tty.h>

// Libc imports
#include <libc/math.h>
#include <libc/string.h>

// File imports
#include <fs/tar.h>

// Leaf header import
#include <sys/leaf.h>

#if !defined(LEAF_LIMINE)
#error "Leaf currently only supports x86_64 with limine!"
#endif

// Limine requests
volatile struct limine_framebuffer_request framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 1};
volatile struct limine_module_request mod_request = {.id = LIMINE_MODULE_REQUEST, .revision = 0};
volatile struct limine_hhdm_request hhdm_request = {.id = LIMINE_HHDM_REQUEST, .revision = 0};

// Global variables
struct limine_framebuffer *framebuffer;
uint64_t hhdm_offset;

// Kernel entry function
void _start(void)
{
    // init code
    hhdm_offset = hhdm_request.response->offset;
    framebuffer = framebuffer_request.response->framebuffers[0];

    init_idt();
    init_pit();
    init_pmm();
    init_tty();
    tty_spawn(0);

    cdebug_log(__func__, "Kernel init finished.");
    dprintf("\n");

    // Print out some system info
    printf("leaf @ tty%04d\n\n", currentTTYid);
    printf("Leaf Version: %s\n", LEAF_VERSION);
    printf("Arch: %s\n", LEAF_ARCH);
    printf("CPU Model: %d\n", get_model());

    char brand[49];
    char vendor_string[13];
    get_intel_cpu_brand_string(brand);
    get_cpu_vendor_string(vendor_string);

    printf("CPU Vendor: %s\n", vendor_string);
    printf("CPU Brand: %s\n", brand);
    printf("Bootloader: %s\n\n", LEAF_BOOTLOADER);

    TAREntry *tar = (TAREntry *)kmalloc(sizeof(TAREntry));
    if (tar == NULL)
    {
        cdebug_log(__func__, "Failed to allocate memory for initrd!");
        hcf();
    }

    TARExtract((char *)(mod_request.response->modules[0]->address), mod_request.response->modules[0]->size, tar);

    printf("TAR Test:\n - %s: %s\n", tar->files[0].name, tar->files[0].content);

    hcf();
}