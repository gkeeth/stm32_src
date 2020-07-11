#ifndef I2C_H
#define I2C_H

#define I2C_PORT GPIOA
#define SCL_PIN GPIO9
#define SDA_PIN GPIO10

/*
 * setup I2C peripheral
 *
 * I2C peripheral: I2C1
 * SCL: PA9
 * SDA: PA10
 */
void i2c_setup(void);

/* set RELOAD bit in I2C_CR2 register */
void i2c_set_reload(uint32_t i2c);

/* clear RELOAD bit in I2C_CR2 register */
void i2c_clear_reload(uint32_t i2c);

/*
 * return status of TCR flag in I2C_ISR register
 *
 * TCR is set when RELOAD=1 and NBYTES have been transferred.
 * TCR is cleared when NBYTES is rewritten to a non-zero value.
 */
bool i2c_transfer_complete_reload(uint32_t i2c);

/* clear NACKF flag in ICR register */
void i2c_clear_nack(uint32_t i2c);

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
        uint8_t *h, size_t hn, uint8_t *w, size_t wn);

#endif
