#include <stdlib.h>
#include <stdint.h>
#include <libopencm3/stm32/i2c.h>

#include "ssd1306.h"
#include "ssd1306_graphics.h"
#include "font8x8_basic.h"

/* fill framebuffer with solid color (PIXEL_OFF, PIXEL_ON, or PIXEL_TOGGLE) */
void fill_display(pixel_t color) {
    for (uint32_t j = 0; j < DISP_HEIGHT / 8; j++) {
        for (uint32_t i = 0; i < DISP_WIDTH; i++) {
            ssd1306_draw_page(i, j, color);
        }
    }
}

/* draw an 8px * 8px checkerboard to framebuffer */
void draw_checkerboard(void) {
    for (uint32_t j = 0; j <= DISP_HEIGHT; j++) {
        for (uint32_t i = 0; i <= DISP_WIDTH; i++) {
            ssd1306_draw_pixel(i, j, ((i/8) % 2 == (j/8) % 2));
        }
    }
}

/*
 * draw a rectangle to the framebuffer
 *
 * x0:    left-most x coordinate of box
 * y0:    upper-most y coordinate of box
 * x1:    right-most x coordinate of box
 * y1:    lower-most y coordinate of box
 * color: color of rectangle (PIXEL_OFF, PIXEL_ON, PIXEL_TOGGLE)
 */
void draw_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
        pixel_t color) {
    for (uint32_t j = y0; j <= y1; j++) {
        for (uint32_t i = x0; i <= x1; i++) {
            ssd1306_draw_pixel(i, j, color);
        }
    }
}

/* draw line with m <= 1 (helper function) */
static void draw_line_small_slope(uint8_t x0, uint8_t y0,
        uint8_t x1, uint8_t y1, pixel_t color) {
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int32_t error = 0;
    int32_t ystep = 1;
    uint8_t y = y0;
    if (dy < 0) {
        ystep = -1;
        dy = -dy;
    }
    int32_t a = 2 * dy - dx;
    for (uint8_t x = x0; x <= x1; x++) {
        ssd1306_draw_pixel(x, y, color);
        if (2 * error + a < 0) {
            error += dy;
        } else {
            error += dy - dx;
            y += ystep;
        }
    }
}

/* draw line with m > 1 (helper function) */
static void draw_line_large_slope(uint8_t x0, uint8_t y0,
        uint8_t x1, uint8_t y1, pixel_t color) {
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int32_t error = 0;
    int32_t xstep = 1;
    uint8_t x = x0;
    if (dx < 0) {
        xstep = -1;
        dx = -dx;
    }
    int32_t a = 2 * dx - dy;
    for (uint8_t y = y0; y <= y1; y++) {
        ssd1306_draw_pixel(x, y, color);
        if (2 * error + a < 0) {
            error += dx;
        } else {
            error += dx - dy;
            x += xstep;
        }
    }
}

/*
 * draw line from (x0, y0) to (x1, y1)
 * Uses Bresenham's algorithm, see:
 * https://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html
 * https://www.en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 */
void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, pixel_t color) {
    if (abs(y1 - y0) > abs(x1 - x0)) {
        /* large slope: 1 < m < inf */
        if (y1 > y0) {
            /* iterate y0 -> y1 */
            draw_line_large_slope(x0, y0, x1, y1, color);
        } else {
            /* iterate y1 -> y0 */
            draw_line_large_slope(x1, y1, x0, y0, color);
        }
    } else {
        /* small slope: 0 <= m <= 1 */
        if (x1 > x0) {
            /* iterate x0 -> x1 */
            draw_line_small_slope(x0, y0, x1, y1, color);
        } else {
            /* iterate x1 -> x0 */
            draw_line_small_slope(x1, y1, x0, y0, color);
        }
    }
}

/* draw one 8x8 character, top left pixel at (x, y) */
void draw_character(char c, uint8_t x, uint8_t y, pixel_t color) {
    for (uint8_t row = 0; row < 8; row++) {
        uint8_t pixels = font8x8_basic[(size_t) c][row];
        for (uint8_t col = 0; col < 8; col++) {
            if ((pixels >> col) & 0x1) {
                ssd1306_draw_pixel(x + col, y + row, color);
            }
        }
    }
}

#define CHAR_HEIGHT 8U
#define CHAR_WIDTH 8U
#define LINE_SPACING 2U
#define XPAD 2U
#define YPAD 2U
/*
 * draw a textbox with a solid background, filled with text (8x8 pixel chars)
 *
 * s:       text to put in box
 * n:       number of characters in s (terminator can be ignored)
 * x0:      x coordinate of top left corner of text box
 * y0:      y coordinate of top left corner of text box
 * x1:      x coordinate of bottom right corner of text box
 * y1:      y coordinate of bottom right corner of text box
 * bgcolor: color of textbox background (black, white, toggle)
 * fgcolor: color of text (black, white, toggle)
 *
 * To fit all text in textbox:
 * x1 = x0 + 4 + 8*chars_in_longest_line
 * y1 = y0 + 4 + 8*number_of_lines + 2*(number_of_lines - 1)
 */
void draw_textbox(char *s, uint32_t nchars, uint32_t x0, uint32_t y0,
        uint32_t x1, uint32_t y1, pixel_t bgcolor, pixel_t fgcolor) {

    draw_rectangle(x0, y0, x1, y1, bgcolor);

    uint8_t x = x0 + XPAD;
    uint8_t y = y0 + YPAD;
    uint32_t n = 0;
    while (n < nchars) {
        if (*s == '\n' || x + CHAR_WIDTH + XPAD > x1) {
            /* go to next line */
            x = x0 + XPAD;
            y += CHAR_HEIGHT + LINE_SPACING;
            if (*s == '\n') { /* don't print newline character */
                s++;
                n++;
                continue;
            }
        }
        if (y + CHAR_HEIGHT + YPAD > y1) {
            break;
        }

        draw_character(*s++, x, y, fgcolor);
        x += CHAR_WIDTH;
        n++;
    }
}
