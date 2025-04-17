#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

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
    
    
    // I2C EXAMPLE
    
    uint8_t ADDR = 0x50; // CHANGE THIS TO YOUR DEVICE'S I2C ADDRESS
    uint8_t reg = 0x00; // CHANGE THIS TO YOUR DEVICE'S REGISTER ADDRESS
    uint8_t buf[2] = {0x00, 0x00}; // CHANGE TO DATA YOU WANT TO WRITE 

    // write
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
    
    // read
    i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, ADDR, buf, 2, false);



    
    while (!stdio_usb_connected()) {  // waits until the USB port has been opened
        sleep_ms(100);
    }
    printf("Start!\n");

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
