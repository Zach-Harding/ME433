#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "font.h"
#include "ssd1306.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9



int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();
    uint8_t t, t1, tdiff;

    while (true) {
        char message[50];
        t = readtime;
        ssd1306_clear();
        sprintf(message, "Hello");
        draw_message(10, 20, message);
        ssd1306_update();
        t2 = readtime;
        tdiff = t2 - t1;
        sleep_ms(1000);
    }
}


void draw_message(int x, int y, char *message) {
    int i = 0;
    while (message[i] != '\0') {
        draw_char(x + (i * 6), y, message[i]);
        i++;
    }
}

void draw_char(int x, int y, char c) {
    int i, j;
    for (i = 0; i < 5; i++) {
        unsigned char line = ASCII[c - 0x20][i];
        for (j = 0; j < 8; j++) {
            // char bit = (col >> j) & 0x01;

            if ((line >> j) & 0x01) {
                ssd1306_drawPixel(x + i, y + j, 1);
            } else {
                ssd1306_drawPixel(x + i, y + j, 0);
            }
        }
    }
}