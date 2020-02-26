#include <stdint.h>
#include <stddef.h>
#include "mm/pmm.h"
#include "sys/apic.h"
#include "sys/int/isr.h"
#include "drivers/pit.h"
#include "drivers/serial.h"
#include "drivers/vesa.h"
#include "drivers/tty/tty.h"
#include "multiboot.h"

/* Testing includes */
#include "proc/scheduler.h"
#include "io/msr.h"

#include "klibc/string.h"

// Kernel main function, execution starts here :D
void kmain(multiboot_info_t *mboot_dat) {
    init_serial(COM1);

    sprintf("\nPAT MSR: %lx", read_msr(0x277));

    if (mboot_dat) {
        sprintf("[DripOS]: Setting up memory bitmaps");
        pmm_memory_setup(mboot_dat);
    }

    sprintf("\n[DripOS] Initializing TTY");
    init_vesa(mboot_dat);
    tty_init(&base_tty, 8, 8);

    sprintf("\n[DripOS] Configuring LAPICs and IOAPIC routing");
    configure_apic();

    //scheduler_init_bsp();
    //tty_clear(&base_tty);

    //sprintf("\n[DripOS] Registering interrupts and setting interrupt flag");
    //configure_idt();
    //sprintf("\n[DripOS] Setting timer speed to 1000 hz");
    //set_pit_freq();

    memcpy32((uint32_t *) __kernel_start, vesa_display_info.actual_framebuffer, vesa_display_info.framebuffer_pixels);

    while (1) {
        asm volatile("hlt");
    }
}