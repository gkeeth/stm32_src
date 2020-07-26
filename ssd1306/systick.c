#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>

#include "systick.h"

static volatile uint32_t counter = 0;

/* increment systick counter variable on each systick interrupt */
void sys_tick_handler(void) {
    ++counter;
}

/* return current value of milliseconds counter (since systick initialized) */
uint32_t millis(void) {
    return counter;
}

/* delay (blocking) for ms milliseconds */
void delay(uint32_t ms) {
    uint32_t t0 = millis();
    while (millis() < t0 + ms);
}

/* setup systick to fire every 1 ms */
void systick_setup(void) {
    STK_CVR = 0; /* clear systick current value to start immediately */

    /* every 1 ms (1000 Hz) */
    systick_set_frequency(1000, rcc_ahb_frequency);
    systick_counter_enable();
    systick_interrupt_enable();
}
