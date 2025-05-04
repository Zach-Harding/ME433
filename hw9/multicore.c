/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"

#define loop 100
#define FLAG_VALUE 123
#define CASE_0 0
#define CASE_1 1
#define CASE_2 2

void core1_entry() {

    //multicore_fifo_push_blocking(FLAG_VALUE);

    uint32_t g = multicore_fifo_pop_blocking();

    if (g == FLAG_VALUE) {
        printf("initialize GPIOs\n");
        adc_init(); // init the adc module
        adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
        adc_select_input(0);
        gpio_init(15); // set GPIO 15 to be output
        gpio_set_dir(15, GPIO_OUT); // set GPIO 15 to be output
        gpio_put(15, 0); // set GPIO 15 to low (off)
        multicore_fifo_push_blocking(loop);

    } else
        printf("Invalid input\n");
    

    while (1) {
        uint32_t b = multicore_fifo_pop_blocking();

        if (b == CASE_0){
            printf("Return volatage on pin A0\n");
            uint16_t result = adc_read(); // read the ADC value
            float voltage = (float)result * 3.3f / (1 << 12); // convert to voltage
            printf("ADC value: %f V\n", voltage);
            sleep_ms(10);
        } else if (b == CASE_1) {
            printf("Turn on an LED on GP15\n");
            gpio_put(15, 1); // set GPIO 15 to high (on)
            sleep_ms(100);
            
        } else if (b == CASE_2) {
            printf("Turn off LED on GP15\n");
            gpio_put(15, 0); // set GPIO 15 to low (off)
            sleep_ms(100);
        } else
            printf("Invalid input\n");
            
        multicore_fifo_push_blocking(loop);
        
        tight_loop_contents();
    }


        
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Hello, multicore!\n");
    multicore_launch_core1(core1_entry);
   
    multicore_fifo_push_blocking(FLAG_VALUE);

    // Wait for it to start up

    

    /// \end::setup_multicore[]

    while (1){
        uint32_t g = multicore_fifo_pop_blocking();

        if (g != loop)
            printf("Hmm, that's not right on core 0!\n");
        else {
            uint16_t case_number;
            printf("Enter case number (0, 1, or 2): ");
            scanf("%d", &case_number);
            printf("\nYou entered case number %d\n", case_number);
            sleep_ms(10);
            multicore_fifo_push_blocking(case_number);
        }

        tight_loop_contents();
    }

       
}
