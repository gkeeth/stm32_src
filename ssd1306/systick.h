#ifndef SYSTICK_H
#define SYSTICK_H

/* return current value of milliseconds counter (since systick initialized) */
uint32_t millis(void);

/* delay (blocking) for ms milliseconds */
void delay(uint32_t ms);

/* setup systick to fire every 1 ms */
void systick_setup(void);

#endif
