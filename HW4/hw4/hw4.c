#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "math.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_CS   14
#define PIN_SCK  18


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
    uint16_t voltage_conversion = (uint16_t)(voltage * 1023 / 3.3); // Convert voltage to int
    // printf("Voltage: %d\n", voltage_conversion);
    desired = desired | (voltage_conversion << 2); // Set the voltage
    // printf("Desired: %x\n", desired);
    data[0] = (desired >> 8) & 0xFF; // MSB
    data[1] = desired & 0xFF; // LSB
    // printf("Data: %x %x\n", data[0], data[1]);
    cs_select(PIN_CS); // Select the chip
    spi_write_blocking(SPI_PORT, data, len); // Write the data to the DAC
    cs_deselect(PIN_CS); // Deselect the chip
}

int main()
{
    stdio_init_all();
    int rate = 1000000;
    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, rate);
    gpio_set_function(PIN_Channel_A, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_Channel_B, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi
    int count = 0;
    int t = 0;
    int r = 100000;
    while (true) {
        // printf("Hello, world!\n");

        // Write to DAC channel A
        writeDAC(0, 1.65 + 1.65*sin(4*3.14*t/r)); // Set channel A to 1.5V
        // Write to DAC channel B
        writeDAC(1, count*(3.3/r)); // Set channel B to 2.5V

        if (count == r) {
            count = 0;
        } else {
            count++;
        }
        t++;
    }
}
