/*
 * Driver for SSD1306 OLED display (I2C)
 */

#include <stdint.h>
#include <libopencm3/stm32/i2c.h>

#include "i2c.h"
#include "ssd1306.h"

static uint8_t framebuffer[DISP_HEIGHT * DISP_WIDTH / 8] = { 0 };

/* initialize display and turn it on */
void ssd1306_init(void) {
    uint8_t control = CONTROL_BYTE_COMMAND;
    uint8_t init_cmd[] = {
        SSD1306_DISPLAY_OFF,
        SSD1306_SET_CLOCK_DIV,
        0x80, /* reset value */
        SSD1306_SET_MUX_RATIO,
        0x3F, /* multiplex ratio for 128x64 */
        SSD1306_SET_DISPLAY_OFFSET,
        0x00,
        SSD1306_SET_DISP_START_LINE | 0x0,
        SSD1306_SET_CHARGE_PUMP,
        SSD1306_CHARGE_PUMP_ON,
        /* set segment remap
         * set com output scan direction
         * set com hardware configuration 0x12
         * set contrast 0xcf
         * set precharge period 0xf1 (internal)
         * set vcomh deselect level 0x40
         */
        /* SSD1306_DISPLAY_ON_IGNORE_RAM, */
        SSD1306_DISPLAY_ON_FOLLOW_RAM,
        /* SSD1306_DISPLAY_INVERTED, */
        SSD1306_DISPLAY_NOT_INVERTED,
        SSD1306_DISPLAY_ON
    };

    i2c_write_with_header(DISP_I2C, DISP_ADDR, &control, sizeof(control),
            init_cmd, sizeof(init_cmd));
}

/* set the value of a single pixel */
void ssd1306_draw_pixel(uint8_t x, uint8_t y, pixel_t color) {
    if (x >= DISP_WIDTH || y >= DISP_HEIGHT) {
        return;
    }

    int n = (y / 8) * DISP_WIDTH + x;
    int s = y % 8;

    if (color == PIXEL_OFF) {
        framebuffer[n] &= ~(0x1 << s);
    } else if (color == PIXEL_ON) {
        framebuffer[n] |= (0x1 << s);
    } else if (color == PIXEL_TOGGLE) {
        framebuffer[n] ^= (0x1 << s);
    }
}

/* set the value of a single page */
void ssd1306_draw_page(uint8_t x, uint8_t p, pixel_t color) {
    if (x >= DISP_WIDTH || p >= DISP_HEIGHT / 8) {
        return;
    }

    int n = p * DISP_WIDTH + x;

    if (color == PIXEL_OFF) {
        framebuffer[n] = 0x00;
    } else if (color == PIXEL_ON) {
        framebuffer[n] = 0xFF;
    } else if (color == PIXEL_TOGGLE) {
        framebuffer[n] ^= 0xFF;
    }
}

/* write contents of framebuffer to display */
bool ssd1306_update_display(void) {
    uint8_t header[] = {
        /* CONTROL_BYTE_COMMAND, */
        SSD1306_SET_MEM_ADDR_MODE,
        SSD1306_MEM_ADDR_MODE_HORIZ,
        SSD1306_SET_COL_ADDR,
        0, /* start column */
        127, /* end column */
        SSD1306_SET_PAGE_ADDR,
        0, /* start page */
        7, /* end page */
    };
    uint8_t control = CONTROL_BYTE_DATA;

    ssd1306_write_command_list(header, sizeof(header));
    /* need to wait for header transaction to finish before writing the
     * framebuffer, because the framebuffer transaction sets RELOAD which will
     * mess up end of header transaction if it hasn't finished */
    while (i2c_busy(DISP_I2C)); /* TODO: is this still necessary? */
    return i2c_write_with_header(DISP_I2C, DISP_ADDR, &control, sizeof(control),
            framebuffer, sizeof(framebuffer));
}

/* write contents of framebuffer to display, one byte per I2C transaction */
void ssd1306_update_display_slow(void) {
    uint8_t header[] = {
        CONTROL_BYTE_COMMAND,
        SSD1306_SET_MEM_ADDR_MODE,
        SSD1306_MEM_ADDR_MODE_HORIZ,
        SSD1306_SET_COL_ADDR,
        0, /* start column */
        127, /* end column */
        SSD1306_SET_PAGE_ADDR,
        0, /* start page */
        7, /* end page */
    };

    i2c_transfer7(DISP_I2C, DISP_ADDR, header, sizeof(header), 0, 0);

    uint8_t data[] = {
        CONTROL_BYTE_DATA,
        0x00
    };

    /* TODO: optimize this into 1 I2C transaction for entire buffer */
    for (size_t i = 0; i < sizeof(framebuffer); i++) {
        data[1] = framebuffer[i];
        i2c_transfer7(DISP_I2C, DISP_ADDR, data, sizeof(data), 0, 0);
    }
}

/* write a single command to display */
void ssd1306_write_command(const uint8_t command) {
    /* TODO: test */
    uint8_t cmd[] = {
        CONTROL_BYTE_COMMAND,
        command
    };
    i2c_transfer7(DISP_I2C, DISP_ADDR, cmd, sizeof(cmd), 0, 0);
}

/* write a list of commands to the display */
void ssd1306_write_command_list(uint8_t *command_list, uint32_t len) {
    /* TODO: test */
    uint8_t control = CONTROL_BYTE_COMMAND;
    i2c_write_with_header(DISP_I2C, DISP_ADDR, &control, sizeof(control),
            command_list, len);
}
