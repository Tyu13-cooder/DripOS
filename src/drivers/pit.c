#include "pit.h"
#include "io/ports.h"
#include "drivers/serial.h"
#include "klibc/stdlib.h"
#include "drivers/vesa.h"
#include "klibc/font.h"

volatile uint64_t global_ticks = 0;

void timer_handler(int_reg_t *r) {
    global_ticks++;
    sprintf("\nTick: %lu", global_ticks);
    color_t white = {255, 255, 255};
    color_t black = {0, 0, 0};
    render_font(font8x8_basic, 'h', (global_ticks % (vesa_display_info.width / 8)) * 8, (global_ticks / (vesa_display_info.width / 8)) * 8, white, black);
    UNUSED(r);
}

void set_pit_freq() {
    uint16_t divisor = 2386;

    /* Caluclate the bytes to send to the PIT,
    where the bytes indicate the frequency */
    uint8_t low = (uint8_t) (divisor & 0xFF);
    uint8_t high = (uint8_t) ((divisor >> 8) & 0xFF);

    /* Send the command to set the frequency */
    port_outb(0x43, 0x36); /* Command port */
    port_outb(0x40, low); /* Low byte of the frequency */
    port_outb(0x40, high); /* High byte of the frequency */
}