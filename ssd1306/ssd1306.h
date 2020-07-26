#ifndef _SSD1306_H_
#define _SSD1306_H_

/*
 * Driver for SSD1306 OLED display (I2C/SPI)
 *
 * I2C interface enabled by defining SSD1306_I2C in makefile
 * SPI interface enabled by defining SSD1306_SPI in makefile
 * interfaces are mutually exclusive
 *
 * TODO: implement the following controller features:
 * - scrolling
 */

#ifdef SSD1306_I2C
#define DISP_I2C I2C1
#define DISP_ADDR 0x3C
#elif defined(SSD1306_SPI)
#define DISP_SPI SPI1
#endif

#define DISP_WIDTH 128
#define DISP_HEIGHT 64

/* Pixel values (colors): black, white, or toggle current value */
typedef enum {
    PIXEL_OFF,
    PIXEL_ON,
    PIXEL_TOGGLE
} pixel_t;


/* initialize display and turn it on */
void ssd1306_init(void);

/* set the value of a single pixel */
void ssd1306_draw_pixel(uint8_t x, uint8_t y, pixel_t value);

/* set the value of a single page */
void ssd1306_draw_page(uint8_t x, uint8_t p, pixel_t color);

/* write contents of framebuffer to display */
bool ssd1306_update_display(void);

/* write contents of framebuffer to display, one byte per I2C transaction */
void ssd1306_update_display_slow(void);

/* write a single command to display */
void ssd1306_write_command(uint8_t command);

/* write a list of commands to the display */
void ssd1306_write_command_list(uint8_t *command_list, uint32_t len);

#ifdef SSD1306_SPI
/* set DC (data/command) pin to data mode */
void ssd1306_set_data(void);

/* set DC (data/command) pin to command mode */
void ssd1306_set_command(void);

/* deassert reset pin */
void ssd1306_deassert_reset(void);

/* assert reset pin */
void ssd1306_assert_reset(void);

/*
 * write a command buffer via SPI
 *
 * handles asserting/deasserting CS and setting DC appropriately
 *
 * w: pointer to buffer of commands
 * wn: number of commands (number of bytes in buffer)
 */
void ssd1306_spi_write_commands(uint8_t *w, size_t wn);

void ssd1306_spi_write_data(uint8_t *w, size_t wn);
#endif /* SSD1306_SPI */

/*
 * control bytes for commands/data
 *
 * send as first byte after slave address
 *
 * 0x00: following bytes are commands/parameters
 *   continuation bit: 0
 *   data/command# bit: 0
 * 0x40: the following bytes are data for display RAM
 *   continuation bit: 0
 *   data/command# bit: 1
 *   (followed by 6 zeroes)
 */
#define CONTROL_BYTE_COMMAND 0x00
#define CONTROL_BYTE_DATA 0x40


/*
 * Fundamental Command Table
 */

/* set display contrast. Followed by byte specifying contrast */
#define SSD1306_SET_CONTRAST 0x81
/* turn display on; display follows RAM contents [reset] */
#define SSD1306_DISPLAY_ON_FOLLOW_RAM 0xA4
/* turn display on; all pixels on regardless of RAM contents */
#define SSD1306_DISPLAY_ON_IGNORE_RAM 0xA5
/* normal display polarity (1 = pixel ON) [reset] */
#define SSD1306_DISPLAY_NOT_INVERTED 0xA6
/* inverse display polarity (0 = pixel ON) */
#define SSD1306_DISPLAY_INVERTED 0xA7
/* turn display off (sleep mode) [reset] */
#define SSD1306_DISPLAY_OFF 0xAE
/* turn display on */
#define SSD1306_DISPLAY_ON 0xAF


/*
 * Scrolling Command Table
 */

/* right/left horizontal scroll
 *
 * followed by 6 bytes:
 *   dummy byte (0x00)
 *   start page address
 *   time interval between scroll steps, in terms of frame frequency
 *      0b000: 5 frames
 *      0b001: 64 frames
 *      0b010: 128 frames
 *      0b011: 256 frames
 *      0b100: 3 frames
 *      0b101: 4 frames
 *      0b110: 25 frames
 *      0b111: 2 frames
 *    end page address
 *    dummy byte (0x00)
 *    dummy byte (0xFF)
 */
#define SSD1306_SCROLL_RIGHT 0x26
#define SSD1306_SCROLL_LEFT 0x27
/* vertical and right/left scroll
 *
 * followed by 5 bytes:
 *   dummy byte
 *   start page address
 *   time interval between scroll steps, in terms of frame frequency
 *      0b000: 5 frames
 *      0b001: 64 frames
 *      0b010: 128 frames
 *      0b011: 256 frames
 *      0b100: 3 frames
 *      0b101: 4 frames
 *      0b110: 25 frames
 *      0b111: 2 frames
 *    end page address
 *    vertical scrolling offset (number of rows)
 */
#define SSD1306_SCROLL_VERT_RIGHT 0x29
#define SSD1306_SCROLL_VERT_LEFT 0x2A
/* deactivate scroll */
#define SSD1306_SCROLL_DEACTIVATE 0x2E
/* activate scroll */
#define SSD1306_SCROLL_ACTIVATE 0x2F
/* set vertical scroll area
 *
 * followed by 2 bytes:
 *   number of rows in top fixed area [reset = 0]
 *   number of rows in scroll area [reset = 64]
 */
#define SSD1306_SCROLL_SET_VERT_AREA 0xA3


 /*
 * Addressing Setting Command Table
 */

/* set memory addressing mode
 *
 * followed by a byte specifying the mode
 *
 * page mode:       0b10
 *   in page mode, columns are incremented automatically but page is constant
 * horizontal mode: 0b00
 *   in horizontal mode, columns are incremented before pages
 * vertical mode:   0b01
 *   in vertical mode, pages are incremented before columns
 */
#define SSD1306_SET_MEM_ADDR_MODE 0x20
#define SSD1306_MEM_ADDR_MODE_PAGE 0x2
#define SSD1306_MEM_ADDR_MODE_HORIZ 0x0
#define SSD1306_MEM_ADDR_MODE_VERT 0x1
/* set start column address for page addressing mode.
 *
 * low: OR with lower nibble of column start address
 * high: OR with upper nibble of column start address
 */
#define SSD1306_SET_COL_START_ADDRESS_LOW 0x0
#define SSD1306_SET_COL_START_ADDRESS_HIGH 0x10
/* set start page address for page addressing mode. OR w/ starting page address */
#define SSD1306_SET_PAGE_START_ADDRESS 0xB0
/* set start/end column and page for horizontal/vertical addressing mode
 *
 * followed by two bytes: start address, end address
 */
#define SSD1306_SET_COL_ADDR 0x21
#define SSD1306_SET_PAGE_ADDR 0x22


/*
 * Hardware Configuration Command Table
 */

/* set display start line. OR w/ segment start line */
#define SSD1306_SET_DISP_START_LINE 0x40
/* set segment remap: col0 mapped to seg0. OR w/ 1 to map col127 to seg0. */
#define SSD1306_SET_SEG_REMAP 0xA0
/* set multiplex ratio; followed by mux ratio byte */
#define SSD1306_SET_MUX_RATIO 0xA8
/* set COM output scan direction to normal */
#define SSD1306_SET_COM_SCAN_DIR_NORM 0xC0
/* set COM output scan direction to remapped */
#define SSD1306_SET_COM_SCAN_DIR_REMAPPED 0xC8
/* set display offset; followed by offset byte */
#define SSD1306_SET_DISPLAY_OFFSET 0xD3
/* set COM pins hardware configuration; followed by config byte */
#define SSD1306_SET_COM_HW_CONFIG 0xDA


/*
 * Timing & Driving Scheme Setting Command Table
 */

/* set clock divide ratio / oscillator frequency */
#define SSD1306_SET_CLOCK_DIV 0xD5
/* set pre-charge period */
#define SSD1306_SET_PRECHARGE_PERIOD 0xD9
/* set V_COMH deselect level */
#define SSD1306_SET_VCOMH_DESELECT_LEV 0xDB
/* NOP */
#define SSD1306_NOP 0xE3


/*
 * Charge Pump Command Table
 */

/* set charge pump setting; followed by a byte specifying the setting
 *   0x10: charge pump disabled [reset]
 *   0x14: charge pump enabled when display is on
 */
#define SSD1306_SET_CHARGE_PUMP 0x8D
#define SSD1306_CHARGE_PUMP_OFF 0x10
#define SSD1306_CHARGE_PUMP_ON 0x14

#endif
