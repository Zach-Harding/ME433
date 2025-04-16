#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define CS_DAC   17
#define PIN_SCK  18
#define CS_RAM  19

void spi_ram_init();
void ram_write(uint16_t addy, float v);
float ram_read(uint16_t addy);
void writeDAC(int channel, float voltage);
void float_math();

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

union FloatInt {
    float f;
    uint32_t i;
};

// EXAMPLE OF HANDLING FLOATINT
// union FloatInt num;
// num.f = 1.23;
// (num.i>>24)&0xFF;

int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(CS_DAC,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(CS_RAM, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(CS_DAC, GPIO_OUT);
    gpio_put(CS_DAC, 1);
    gpio_set_dir(CS_RAM, GPIO_OUT);
    gpio_put(CS_RAM, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    spi_ram_init();

    while (!stdio_usb_connected()) {  // waits until the USB port has been opened
        sleep_ms(100);
    }
    printf("Start!\n");

    float_math();
    

    // for (int i = 0; i<1000; i++) {
    //     union FloatInt num;
    //     num.f = 1.65 + 1.65*sin(4*3.14*i/1000);
    //     uint16_t addy = XXX;
    //     ram_write(addy, num.f);
    // }    

    while (true) {
        // read for one address
        
        
        // send float to dac
        // writeDAC(0, num.f);

        sleep_ms(10);
    }
}


void spi_ram_init() {
    uint8_t buf[2];
    buf[0] = 0b00000101; // i want to change reg
    buf[1] = 0b01000000; // sequential mode
    
    cs_select(CS_RAM); // cs low
    spi_write_blocking(spi_default, buf, 2);
    cs_deselect(CS_RAM); // cs high
}

void ram_write(uint16_t addy, float v) {
    uint8_t buf[7];
    buf[0] = 0b00000010; // write command
    buf[1] = (addy>>8)&0xFF; // address high byte
    buf[2] = addy&0xFF; // address low byte
    union FloatInt num;
    num.f = v;
    buf[3] = (num.i>>24)&0xFF; // data high byte
    buf[4] = (num.i>>16)&0xFF; // data high byte
    buf[5] = (num.i>>8)&0xFF; // data high byte
    buf[6] = num.i&0xFF; // data low byte
    
    cs_select(CS_RAM); // cs low
    spi_write_blocking(spi_default, buf, 7);
    cs_deselect(CS_RAM); // cs high
}

float ram_read(uint16_t addy) {
    uint8_t obuf[7], ibuf[7];
    union FloatInt num;
    obuf[0] = 0b00000011; // read command
    obuf[1] = (addy>>8)&0xFF; // address high byte
    obuf[2] = addy&0xFF; // address low byte


    cs_select(CS_RAM); // cs low
    spi_write_read_blocking(spi_default, obuf, ibuf, 7);
    cs_deselect(CS_RAM); // cs high

    num.i = (ibuf[3]<<24) | (ibuf[4]<<16) | (ibuf[5]<<8) | ibuf[6];

    return num.f;
}

void float_math(){
    volatile float f1, f2;
    printf("Enter two floats to use:");
    scanf("%f %f", &f1, &f2);
    printf("\n");
    volatile float f_add, f_sub, f_mult, f_div;
    absolute_time_t t_start, t_end_add, t_end_sub, t_end_mult, t_end_div;
    
    
    t_start = get_absolute_time(); 
    
    for(int i = 0; i<1000; i++) {
        f_add = f1+f2;
    }
    t_end_add = get_absolute_time(); 
    
    for(int i = 0; i<1000; i++) {
        f_sub = f1-f2;
    }
    t_end_sub = get_absolute_time(); 
    
    for(int i = 0; i<1000; i++) {
        f_mult = f1*f2;
    }

    t_end_mult = get_absolute_time(); 
    
    for(int i = 0; i<1000; i++) {
        f_div = f1/f2;
    }
    t_end_div = get_absolute_time(); 
    

    double add_time = (double)absolute_time_diff_us(t_start, t_end_add);
    double sub_time = (double)absolute_time_diff_us(t_end_add, t_end_sub);
    double mult_time = (double)absolute_time_diff_us(t_end_sub, t_end_mult);
    double div_time = (double)absolute_time_diff_us(t_end_mult, t_end_div);

    printf("Add cycles: %.2f cycles\n", add_time*1000/1000/6.67);
    printf("Sub time: %f cycles\n", sub_time*1000/1000/6.67);
    printf("Mult time: %f cycles\n", mult_time*1000/1000/6.67);
    printf("Div time: %f cycles\n", div_time*1000/1000/6.67);
    printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", 
    f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);
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
    cs_select(CS_DAC); // Select the chip
    spi_write_blocking(SPI_PORT, data, len); // Write the data to the DAC
    cs_deselect(CS_DAC); // Deselect the chip
}