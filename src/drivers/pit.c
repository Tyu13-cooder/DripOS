#include "pit.h"
#include "io/ports.h"
#include "sys/smp.h"
#include "drivers/serial.h"
#include "drivers/tty/tty.h"
#include "klibc/stdlib.h"
#include "proc/scheduler.h"
#include "proc/sleep_queue.h"

volatile uint64_t global_ticks = 0;

void timer_handler(int_reg_t *r) {
    global_ticks++;
    advance_time(); // Sleep queue

    if (global_ticks % sched_period == 0 && scheduler_enabled) {
        schedule_bsp(r);
    }
}

void set_pit_freq() {
    uint16_t divisor = 1193;

    /* Caluclate the bytes to send to the PIT,
    where the bytes indicate the frequency */
    uint8_t low = (uint8_t) (divisor & 0xFF);
    uint8_t high = (uint8_t) ((divisor >> 8) & 0xFF);

    /* Send the command to set the frequency */
    port_outb(0x43, 0x36); /* Command port */
    port_outb(0x40, low); /* Low byte of the frequency */
    port_outb(0x40, high); /* High byte of the frequency */
}

void sleep_no_task(uint64_t ticks) {
    volatile uint64_t start_ticks = global_ticks;
    while (global_ticks < ticks + start_ticks) asm volatile("pause");
}

uint64_t stopwatch_start() {
    return global_ticks;
}

uint64_t stopwatch_stop(uint64_t start) {
    return (global_ticks - start);
}

void time_code(uint64_t *start, char *description) {
    sprintf("%s Took: %lu\n", description, global_ticks - *start);
    *start = global_ticks;
}