/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define GPIO_WATCH_PIN 2
#define LED_PIN 16 // GPIO pin for the LED

static char event_str[128];

void gpio_event_string(char *buf, uint32_t events);

int pico_led_init(void) {
    #if defined(LED_PIN)
        // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
        // so we can use normal GPIO functionality to turn the led on and off
        gpio_init(LED_PIN);
        gpio_set_dir(LED_PIN, GPIO_OUT);
        return PICO_OK;
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        // For Pico W devices we need to initialise the driver etc
        return cyw43_arch_init();
    #endif
    }
    
    // Turn the led on or off
    void pico_set_led(bool led_on) {
    #if defined(LED_PIN)
        // Just set the GPIO on or off
        gpio_put(LED_PIN, led_on);
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        // Ask the wifi "driver" to set the GPIO on or off
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
    #endif
} 

void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    gpio_event_string(event_str, events);
    //printf("GPIO %d %s\n", gpio, event_str);
}



int main() {
    stdio_init_all();
    pico_led_init();
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    while (!stdio_usb_connected()) {  // waits until the USB port has been opened
        sleep_ms(100);
    }
    printf("Start!\n");
    printf("ADC initial value: %d\n", adc_read()); 

    gpio_init(GPIO_WATCH_PIN);
    gpio_set_irq_enabled_with_callback(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    // Infinite loop to keep the program running
    while (1) {
        // TODO:
        // Turns on an LED
        pico_set_led(true);
        printf("Press the button to continue...\n");
        while(!gpio_get(GPIO_WATCH_PIN)) { // Waits until a button is pressed
            sleep_ms(50);
        }
        // Turns off the LED
        pico_set_led(false); // Turns off the LED
        sleep_ms(50); 
        

        // Asks the user to enter a number of analog samples to take, between 1 and 100
        // Reads the number entered by the user
        uint16_t i;
        printf("Enter a Number from 0 to 100: ");
        scanf("%d", &i);
        printf("Number: %d\r\n",i);
        sleep_ms(50);

        // Reads the voltage on ADC0 that number of times at 100Hz
        for(int z = 0; z < i; z++) { 
            uint16_t result = adc_read(); 
            float voltage = (float)result * 3.3f / (1 << 12); 
            printf("ADC0: %f V\r\n", voltage); 
            sleep_ms(10); // wait for 10ms before taking the next sample
        }
        
    }
}


static const char *gpio_irq_str[] = {
        "LEVEL_LOW",  // 0x1
        "LEVEL_HIGH", // 0x2
        "EDGE_FALL",  // 0x4
        "EDGE_RISE"   // 0x8
};

void gpio_event_string(char *buf, uint32_t events) {
    for (uint i = 0; i < 4; i++) {
        uint mask = (1 << i);
        if (events & mask) {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0') {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events) {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}
