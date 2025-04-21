#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 20
#define I2C_SCL 21
#define I2C_ADDR 0x20 // I2C address of the device
#define IODIR 0x00 // I2C register for IODIR
#define GPIO 0x09 // I2C register for GPIO
#define OLAT 0x0A // I2C register for OLAT

static void iodir_init();
static void olat_init();
static void set_pin(uint8_t addr, uint8_t reg, uint8_t pin);
static uint8_t read_pin(uint8_t addr, uint8_t reg);



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

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    //gpio_pull_up(I2C_SDA);
    //gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c
    
    // set up iodir
    iodir_init(); 
    // set up GPIO
    olat_init();
    // LED initialisation
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    uint8_t val;

    

    // ASSIGNMENT
    // The goal is to read from GP0, and if the button is pushed, 
    // turn on GP7, else turn off GP7. Blink the builtin green LED 
    // that is connected directly to the Pico at some frequency as 
    // a heart-beat, so that you know the Pico is running, and 
    // hasn't crashed due to the I2C getting our of sync.


    
    while (!stdio_usb_connected()) {  // waits till port is open
        sleep_ms(100);
    }
    printf("Start!\n");
    val = read_pin(I2C_ADDR, GPIO); // read the state of the GPIO pins
    printf("GPIO: %x\n", val);

    set_pin(I2C_ADDR, OLAT, 0b10000000); // set GP7 high

    sleep_ms(100);
    val = read_pin(I2C_ADDR, GPIO); // read the state of the GPIO pins
    printf("GPIO: %d\n", val);

   

    while (true) {
        // clear_pin(I2C_ADDR, OLAT, 7); // set GP7 low
        pico_set_led(true);
        sleep_ms(100);
        pico_set_led(false);
        sleep_ms(100);

        val = read_pin(I2C_ADDR, GPIO); // read the state of the GPIO pins
        printf("GPIO: %x\n", val);

        

    }
}


static void iodir_init() {
    uint8_t buf[2];
    buf[0] = IODIR; // register address
    buf[1] = 0b01111111; // set all pins to input except GP7
    i2c_write_blocking(i2c_default, I2C_ADDR, buf, 2, false);
}

static void olat_init() {
    uint8_t buf[2];
    buf[0] = OLAT; // register address
    buf[1] = 0b00000000; // set all pins to low
    i2c_write_blocking(i2c_default, I2C_ADDR, buf, 2, false);
}

static void set_pin(uint8_t addr, uint8_t reg, uint8_t value) {
    uint8_t buf[2];
    buf[0] = reg; // register address
    buf[1] = value; // value to set
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
}

static uint8_t read_pin(uint8_t addr, uint8_t reg) {
    uint8_t buf;
    i2c_write_blocking(i2c_default, addr, &reg, 1, true);
    i2c_read_blocking(i2c_default, addr, &buf, 1, false);
    return buf; // return the value read from the register
}