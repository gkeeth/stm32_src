#ifndef SPI_H
#define SPI_H

#define SPI_PORT GPIOB
#define MOSI_PIN GPIO5
#define SCK_PIN GPIO3

#define CS_PORT GPIOA
#define CS_PIN GPIO5
#define DC_PORT GPIOA
#define DC_PIN GPIO4
#define RESET_PORT GPIOA
#define RESET_PIN GPIO3

/*
 * setup SPI peripheral
 *
 * SPI peripheral: SPI1
 * SCK: PB3
 * MOSI: PB5
 */
void spi_setup(void);

/*
 * write a buffer via SPI, bytewise (8 bit data)
 *
 * spi: SPI peripheral, e.g. SPI1
 * w: pointer to buffer to be written
 * wn: number of bytes in buffer to be written
 */
void spi_write_buffer8(uint32_t spi, uint8_t *w, size_t wn);

#endif
