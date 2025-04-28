#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "font.h"
#include "ssd1306.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

void draw_message(int x, int y, char *message);
void draw_char(int x, int y, char c);

int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}
        
// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}


int main()
{
    stdio_init_all();
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    //gpio_pull_up(I2C_SDA);
    //gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();
    double update_time;
    absolute_time_t t_start, t_end;

    // while (!stdio_usb_connected()) {  // waits until the USB port has been opened
    //     sleep_ms(100);
    // }
    // printf("Start!\n");
    

    while (true) {
        pico_set_led(true);
        sleep_ms(100);
        pico_set_led(false);
        sleep_ms(100);


        ssd1306_clear();
        printf("clear\n");
        ssd1306_update();
        sleep_ms(100);
        ssd1306_drawPixel(10, 10, 1);
        printf("drawPixel\n");
        ssd1306_update();
        sleep_ms(100);



        // char message[50];
        // t_start = get_absolute_time(); 
        // ssd1306_clear();
        // sprintf(message, "Hello");
        // draw_message(10, 20, message);
        // ssd1306_update();
        // t_end = get_absolute_time(); 
        // update_time = (double)absolute_time_diff_us(t_start, t_end);
        // sleep_ms(100);
        // printf("Update time: %f ms\n", update_time / 1000.0);
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