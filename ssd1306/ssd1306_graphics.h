#ifndef SSD1306_GRAPHICS_H
#define SSD1306_GRAPHICS_H

/*
 * Graphics drawing functions for SSD1306 display
 *
 * unimplemented functions (ideas):
 * - draw_circle()
 * - draw_triangle()
 * - draw_polygon()
 * - fill_shape()
 * - textbox with and without parameters (have a sane default option)
 */

/* fill framebuffer with solid color (PIXEL_OFF, PIXEL_ON, or PIXEL_TOGGLE) */
void fill_display(pixel_t color);

/* draw an 8px * 8px checkerboard to framebuffer */
void draw_checkerboard(void);

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
        pixel_t color);

/*
 * draw line from (x0, y0) to (x1, y1)
 * Uses Bresenham's algorithm, see:
 * https://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html
 */
void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, pixel_t color);

/* draw one 8x8 character, top left pixel at (x, y) */
void draw_character(char c, uint8_t x, uint8_t y, pixel_t color);

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
        uint32_t x1, uint32_t y1, pixel_t bgcolor, pixel_t fgcolor);
#endif
