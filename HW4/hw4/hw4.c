#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_Channel_A 15
#define PIN_CS   14
#define PIN_SCK  18
#define PIN_Channel_B 19

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void writeDAC(int channel, float voltage) {
    uint8_t data[2];
    int len = 2;
    uint16_t desired = 0;
    desired = desired | (channel << 15); // Set the channel
    desired = desired | (0b111 << 12); 
    uint16_t voltage_conversion = (uint16_t)(voltage * 4095 / 3.3); // Convert voltage to int
    desired = desired | (voltage_conversion << 2); // Set the voltage
    data[0] = (desired >> 8) & 0xFF; // MSB
    data[1] = desired & 0xFF; // LSB
    cs_select(PIN_CS); // Select the chip
    spi_write_blocking(SPI_PORT, data, len); // Write the data to the DAC
    cs_deselect(PIN_CS); // Deselect the chip
}

int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000);
    gpio_set_function(PIN_Channel_A, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_Channel_B, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    while (true) {
        printf("Hello, world!\n");

        sleep_ms(1000);
    }
}
