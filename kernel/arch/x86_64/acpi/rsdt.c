#include <arch/x86_64/acpi/acpi.h>
#include <arch/x86_64/acpi/fadt.h>
#include <arch/x86_64/acpi/madt.h>
#include <arch/x86_64/acpi/mcfg.h>
#include <arch/x86_64/acpi/rsdt.h>

xsdt_t *g_xsdt;
rsdt_t *g_rsdt;

void init_rsdt() {
    rsdp_t *rsdp = (rsdp_t *)rsdp_request.response->address;

    g_rsdt = (rsdt_t *)(uintptr_t)PHYS_TO_VIRT(rsdp->rsdt_addr);
    if(_use_xsdt()) {
        vcplog("Using XSDT");
        xsdp_t *xsdp = (xsdp_t *)rsdp_request.response->address;
        g_xsdt = (xsdt_t *)(uintptr_t)PHYS_TO_VIRT(xsdp->xsdt_addr);
    }

    madt_t *madt = _find_sdt("APIC");
    if(madt == NULL) {

        plog("Failed to find MADT!");
        hcf();
    }

    init_madt(madt);
    fadt_t *fadt = _find_sdt("FACP");
    if(madt == NULL) {

        plog("Failed to find FADT!");
        hcf();
    }

    init_fadt(fadt);

    mcfg_t *mcfg = _find_sdt("MCFG");
    if(mcfg == NULL) {

        plog("Failed to find MCFG!");
        hcf();
    }

    init_mcfg(mcfg);

    vcplog("CPU Cores: %d", g_acpi_cpu_count);
    vvcplog("done.");
}
