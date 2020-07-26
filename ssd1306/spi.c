#include <stddef.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include "spi.h"

/*
 * setup SPI peripheral
 *
 * SPI peripheral: SPI1
 * SCK: PB3
 * MOSI: PB5
 */
void spi_setup(void) {
    rcc_periph_clock_enable(RCC_SPI1);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);

    gpio_mode_setup(SPI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SCK_PIN | MOSI_PIN);
    gpio_set_af(SPI_PORT, GPIO_AF0, SCK_PIN | MOSI_PIN);

    /* TODO: setup for CS, DC, RESET should go into a different function */
    gpio_mode_setup(CS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CS_PIN);
    gpio_mode_setup(DC_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, DC_PIN);
    gpio_mode_setup(RESET_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, RESET_PIN);
    /* initially deselect display, hold it in reset, and set control mode */
    gpio_set(CS_PORT, CS_PIN);
    gpio_clear(RESET_PORT, RESET_PIN);
    gpio_clear(DC_PORT, DC_PIN);


    /* TODO: figure out baud rate */
    spi_init_master(SPI1,
            SPI_CR1_BAUDRATE_FPCLK_DIV_16,
            SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
            SPI_CR1_CPHA_CLK_TRANSITION_2,
            SPI_CR1_MSBFIRST);
    spi_enable_software_slave_management(SPI1); /* set SSM, clear SSOE */
    spi_set_nss_high(SPI1); /* set SSI */
    spi_set_data_size(SPI1, SPI_CR2_DS_8BIT); /* set DS[3:0] */

    spi_enable(SPI1);
}

/*
 * write a buffer via SPI, bytewise (8 bit data)
 *
 * CS pin must be asserted/deasserted externally
 *
 * Blocks (internally to spi_send8())
 *
 * spi: SPI peripheral, e.g. SPI1
 * w: pointer to buffer to be written
 * wn: number of bytes in buffer to be written
 */
void spi_write_buffer8(uint32_t spi, uint8_t *w, size_t wn) {
    for (size_t n = 0; n < wn; n++) {
        spi_send8(spi, w[n]);
    }
}
