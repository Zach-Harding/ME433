#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

// DRV8835 Pin Configuration
#define MOTOR1_IN1 16  // GPIO16
#define MOTOR1_IN2 17  // GPIO17
#define MOTOR2_IN1 18  // GPIO18
#define MOTOR2_IN2 19  // GPIO19

// PWM Configurations
#define PWM_FREQ 20000  // 20 kHz (inaudible frequency)
#define PWM_WRAP 1000    // Allows duty cycle from 0-100%

// Motor Control Structure
typedef struct {
    uint in1_pin;
    uint in2_pin;
    uint in1_slice;
    uint in1_channel;
    uint in2_slice;
    uint in2_channel;
    int duty_cycle;
} MotorController;


void setup_pwm(uint pin, uint *slice, uint *channel);
void init_motor(MotorController *motor, uint in1, uint in2);
void update_motor(MotorController *motor);

MotorController motor1, motor2;

void setup_pwm(uint pin, uint *slice, uint *channel) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    *slice = pwm_gpio_to_slice_num(pin);
    *channel = pwm_gpio_to_channel(pin);
    
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, PWM_WRAP);
    pwm_config_set_clkdiv(&config, 1.0f);  // No clock division
    pwm_init(*slice, &config, true);
}

void init_motor(MotorController *motor, uint in1, uint in2) {
    motor->in1_pin = in1;
    motor->in2_pin = in2;
    motor->duty_cycle = 0;
    
    setup_pwm(in1, &motor->in1_slice, &motor->in1_channel);
    setup_pwm(in2, &motor->in2_slice, &motor->in2_channel);
    
    update_motor(motor);
}

void update_motor(MotorController *motor) {
    // Limit duty cycle to ±1000
    if (motor->duty_cycle > 1000) motor->duty_cycle = 1000;
    if (motor->duty_cycle < -1000) motor->duty_cycle = -1000;

    if (motor->duty_cycle > 0) {
        // Forward direction
        pwm_set_chan_level(motor->in1_slice, motor->in1_channel, motor->duty_cycle);
        pwm_set_chan_level(motor->in2_slice, motor->in2_channel, 0);
    } else if (motor->duty_cycle < 0) {
        // Reverse direction
        pwm_set_chan_level(motor->in1_slice, motor->in1_channel, 0);
        pwm_set_chan_level(motor->in2_slice, motor->in2_channel, -motor->duty_cycle);
    } else {
        // Brake (both pins low)
        pwm_set_chan_level(motor->in1_slice, motor->in1_channel, 0);
        pwm_set_chan_level(motor->in2_slice, motor->in2_channel, 0);
    }
}

void print_help() {
    printf("\nDC Motor Control with DRV8835\n");
    printf("Commands:\n");
    printf("  +   - Increase duty cycle by 1%%\n");
    printf("  -   - Decrease duty cycle by 1%%\n");
    printf("  s   - Stop motor (0%% duty cycle)\n");
    printf("  h   - Print this help message\n");
    printf("Current duty cycle: %d%%\n", motor1.duty_cycle);
}

int main() {
    stdio_init_all();
    
    // Initialize motors
    init_motor(&motor1, MOTOR1_IN1, MOTOR1_IN2);
    init_motor(&motor2, MOTOR2_IN1, MOTOR2_IN2);

    while (!stdio_usb_connected()) {  // waits until the USB port has been opened
        sleep_ms(100);
    }
    
    printf("DC Motor Control Initialized\n");
    print_help();
    
    while (true) {
        int c = getchar();
        if (c != PICO_ERROR_TIMEOUT) {
            switch (c) {
                case '+':
                    motor1.duty_cycle += 5;
                    update_motor(&motor1);
                    printf("Duty cycle: %d%%\n", motor1.duty_cycle);
                    motor2.duty_cycle += 5;
                    update_motor(&motor2);
                    printf("Duty cycle: %d%%\n", motor2.duty_cycle);
                    break;
                case '-':
                    motor1.duty_cycle -= 5;
                    update_motor(&motor1);
                    printf("Duty cycle: %d%%\n", motor1.duty_cycle);
                    motor2.duty_cycle -= 5;
                    update_motor(&motor2);
                    printf("Duty cycle: %d%%\n", motor2.duty_cycle);
                    break;
                case 's':
                    motor1.duty_cycle = 0;
                    update_motor(&motor1);
                    printf("Motor stopped\n");
                    motor2.duty_cycle = 0;
                    update_motor(&motor2);
                    break;
                case 'h':
                    print_help();
                    break;
                case '\n':
                case '\r':
                    break;  // Ignore newline characters
                default:
                    printf("Unknown command. Press 'h' for help.\n");
            }
        }
        
        // Small delay to prevent busy waiting
        sleep_ms(10);
    }
    
    return 0;
}