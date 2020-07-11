#include <stdint.h>
#include <stddef.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>

#include "i2c.h"

/*
 * setup I2C peripheral
 *
 * I2C peripheral: I2C1
 * SCL: PA9
 * SDA: PA10
 */
void i2c_setup(void) {
    /* enable I2C
     *
     * enable peripheral clock
     * configure noise filters
     * set PE bit in I2C_CR1 reg
     */
    rcc_periph_clock_enable(RCC_I2C1);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_set_i2c_clock_hsi(I2C1);

    i2c_reset(I2C1);

    /* configure PA9/10 in AF4 for SCL/SDA */
    gpio_mode_setup(I2C_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SCL_PIN | SDA_PIN);
    gpio_set_af(I2C_PORT, GPIO_AF4, SCL_PIN | SDA_PIN);

    /* enable filters */
    i2c_peripheral_disable(I2C1);
    i2c_enable_analog_filter(I2C1);
    i2c_set_digital_filter(I2C1, 0);

    i2c_set_speed(I2C1, i2c_speed_sm_100k, 8); /* 8MHz HSI clock */
    i2c_set_7bit_addr_mode(I2C1);

    i2c_peripheral_enable(I2C1);
    I2C_CR2(I2C1) &= ~I2C_CR2_RELOAD;
}

/* set RELOAD bit in I2C_CR2 register */
void i2c_set_reload(uint32_t i2c) {
    I2C_CR2(i2c) |= I2C_CR2_RELOAD;
}

/* clear RELOAD bit in I2C_CR2 register */
void i2c_clear_reload(uint32_t i2c) {
    I2C_CR2(i2c) &= ~I2C_CR2_RELOAD;
}

/*
 * return status of TCR flag in I2C_ISR register
 *
 * TCR is set when RELOAD=1 and NBYTES have been transferred.
 * TCR is cleared when NBYTES is rewritten to a non-zero value.
 */
bool i2c_transfer_complete_reload(uint32_t i2c) {
    return (I2C_ISR(i2c) & I2C_ISR_TCR);
}

/* clear NACKF flag in ICR register */
void i2c_clear_nack(uint32_t i2c) {
    I2C_ICR(i2c) &= ~I2C_ICR_NACKCF;
}

/*
 * Write a header, followed by another buffer, in 1 transaction.
 *
 * Header must be <= 255 bytes. Buffer size is unlimited.
 *
 * i2c:  I2C peripheral, e.g. I2C1
 * addr: 7bit I2C device address
 * h:    pointer to header to be written
 * hn:   number of bytes in header to be written (255 bytes or fewer)
 * w:    pointer to primary buffer to be written
 * wn:   number of bytes in buffer to be written
 *
 * Returns true on success, false otherwise
 */
bool i2c_write_with_header(uint32_t i2c, uint8_t addr,
        uint8_t *h, size_t hn, uint8_t *w, size_t wn) {

    if (hn > 0xFF) {
        return false;
    }

    /* check that any previous transactions have ended before setting RELOAD.
     * If a previous transaction hasn't finished when RELOAD is set, the STOP
     * will never get written because RELOAD disables autoend */
    if (i2c_busy(i2c)) {
        return false;
    }

    i2c_set_7bit_address(i2c, addr);
    i2c_set_write_transfer_dir(i2c);
    i2c_set_bytes_to_transfer(i2c, hn); /* initially transferring header */
    i2c_enable_autoend(i2c); /* has no effect while RELOAD is set */
    i2c_set_reload(i2c);
    i2c_send_start(i2c);

    /* TODO: collapse into one loop */
    for (size_t n = 0; n < hn; n++) {
        /* wait until ready for another byte to be written to data reg */
        bool wait = true;
        while (wait) {
            if (i2c_transmit_int_status(i2c)) {
                wait = false;
            }
            if (i2c_nack(i2c)) {
                i2c_clear_nack(i2c);
                return false; /* TODO: handle better? */
            }
        }

        i2c_send_data(i2c, *(h + n));
    }

    /* now send main buffer */
    /* size_t total_bytes = sizeof(framebuffer); */
    /* uint8_t *p = framebuffer; */
    uint8_t nbytes = 0;

    /* wait for transfer to complete before reloading NBYTES */
    while (!i2c_transfer_complete_reload(i2c));
    if (wn > 0xFF) {
        nbytes = 0xFF;
        i2c_set_bytes_to_transfer(i2c, nbytes);
    } else {
        nbytes = wn;
        i2c_set_bytes_to_transfer(i2c, nbytes);
        i2c_clear_reload(i2c);
    }

    do {
        bool wait = true;
        while (wait) {
            if (i2c_transmit_int_status(i2c)) {
                wait = false;
            }
            if (i2c_nack(i2c)) {
                i2c_clear_nack(i2c);
                return false; /* TODO: handle better? */
            }
        }

        i2c_send_data(i2c, *w++);
        nbytes--;
        wn--;

        if (nbytes == 0 && wn != 0) {
            /* wait for TCR */
            while (!i2c_transfer_complete_reload(i2c));

            /* reload NBYTES and start transferring the next chunk */
            if (wn > 0xFF) {
                nbytes = 0xFF;
                i2c_set_bytes_to_transfer(i2c, nbytes);
            } else {
                nbytes = wn;
                /* have to set NBYTES before setting RELOAD. Not clear why. */
                i2c_set_bytes_to_transfer(i2c, nbytes);
                i2c_clear_reload(i2c);
            }
        }
    } while (wn > 0);

    return false; /* should never get here */
}
