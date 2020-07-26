#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>

#include "systick.h"

#ifdef SSD1306_I2C
#include "i2c.h"
#elif defined SSD1306_SPI
#include "spi.h"
#endif

#include "ssd1306.h"
#include "ssd1306_graphics.h"

static void setup(void) {
    /* external 8MHz oscillator */
    rcc_osc_bypass_enable(RCC_HSE);
    rcc_clock_setup_in_hse_8mhz_out_48mhz();

    systick_setup();
#ifdef SSD1306_I2C
    i2c_setup();
#elif defined(SSD1306_SPI)
    spi_setup();
#endif
}

int main(void) {
    setup();

    ssd1306_init();
    ssd1306_update_display();


    uint32_t delay_time = 200;
    for (int n = 0; n < 5; n++) {
        fill_display(PIXEL_ON);
        ssd1306_update_display();
        delay(delay_time);

        draw_checkerboard();
        ssd1306_update_display();
        delay(delay_time);

        fill_display(PIXEL_OFF);
        ssd1306_update_display();
        delay(delay_time);

        draw_checkerboard();
        fill_display(PIXEL_TOGGLE);
        ssd1306_update_display();
        delay(delay_time);

        /* top left */
        ssd1306_draw_pixel(0, 0, PIXEL_ON);
        ssd1306_draw_pixel(4, 1, PIXEL_ON);
        /* bottom left */
        ssd1306_draw_pixel(0, 63, PIXEL_ON);
        /* bottom right */
        ssd1306_draw_pixel(127, 63, PIXEL_TOGGLE);
        draw_rectangle(2, 1, 31, 9, PIXEL_TOGGLE);
        draw_rectangle(2, 35, 127, 37, PIXEL_TOGGLE);
        ssd1306_draw_pixel(4, 1, PIXEL_TOGGLE);
        ssd1306_update_display();
        delay(delay_time);
    }

    fill_display(PIXEL_OFF);
    ssd1306_update_display();

    draw_line(0, 32, 127, 32, PIXEL_ON); /* horizontal */
    draw_line(63, 0, 63, 63, PIXEL_ON); /* vertical */

    draw_line(0, 0, 127, 63, PIXEL_ON); /* small positive slope */
    draw_line(64, 0, 127, 63, PIXEL_ON); /* slope +1 */
    draw_line(96, 0, 127, 63, PIXEL_ON); /* large positive slope */
    draw_line(127, 63, 0, 32, PIXEL_ON); /* small positive slope, backwards */
    draw_line(127, 63, 112, 0, PIXEL_ON); /* large positive slope, backwards */

    draw_line(0, 63, 127, 0, PIXEL_ON); /* small negative slope */
    draw_line(0, 63, 63, 0, PIXEL_ON); /* slope -1 */
    draw_line(0, 63, 32, 0, PIXEL_ON); /* large negative slope */
    draw_line(16, 0, 0, 63, PIXEL_ON); /* large negative slope, backwards */
    draw_line(127, 32, 0, 63, PIXEL_ON); /* small negative slope, backwards */
    ssd1306_update_display();

    draw_textbox("two \nlines", 10, 2, 2, 46, 24, PIXEL_OFF, PIXEL_ON);
    draw_textbox("three\nlines\nnow!", 16, 2, 30, 46, 62, PIXEL_ON, PIXEL_OFF);
    ssd1306_update_display();
}
