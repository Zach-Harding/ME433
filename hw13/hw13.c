#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "font.h"
#include "ssd1306.h"
#include "hardware/adc.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75




void draw_message(int x, int y, char *message);
void draw_char(int x, int y, char c);
void read_sensor_data(float *accel_x, float *accel_y, float *accel_z, float *gyro_x, float *gyro_y, float *gyro_z, float *temperature);
void draw_accel_line(float x, float y);

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

    
    printf("Start!\n");
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    //gpio_pull_up(I2C_SDA);
    //gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // printf("before setup\n");
    ssd1306_setup();
    // printf("after setup\n");
    ssd1306_clear();
    // printf("clear\n");
    ssd1306_update();
    // printf("update\n");
    double update_time;
    absolute_time_t t_start, t_end;
    // printf("after setup\n");



    // Set the PWR_MGMT_1 register to 0x00 to wake up the MPU6050
    uint8_t buf[2];
    buf[0] = PWR_MGMT_1;
    buf[1] = 0x00; // Wake up the MPU6050
    i2c_write_blocking(I2C_PORT, 0x68, buf, 2, false);

    buf[0] = ACCEL_CONFIG;
    buf[1] = 0x00; // ±2g
    i2c_write_blocking(I2C_PORT, 0x68, buf, 2, false);
    

    buf[0] = GYRO_CONFIG;
    buf[1] = 0x18; // ±2000 dps
    i2c_write_blocking(I2C_PORT, 0x68, buf, 2, false);

     float accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z, temperature;

    while (true) {
        pico_set_led(true);


        char message[50];
        t_start = get_absolute_time(); 
        ssd1306_clear();

        read_sensor_data(&accel_x, &accel_y, &accel_z, &gyro_x, &gyro_y, &gyro_z, &temperature);
        // sprintf(message, "AX: %.2f g", accel_x);
        // draw_message(0, 0, message);
        // sprintf(message, "AY: %.2f g", accel_y);
        // draw_message(0, 10, message);
        // sprintf(message, "Temp: %.2f C", temperature);
        // draw_message(0, 20, message);
        draw_accel_line(accel_x, accel_y);

        ssd1306_update();
        t_end = get_absolute_time(); 
        update_time = (double)absolute_time_diff_us(t_start, t_end);
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

void read_sensor_data(float *accel_x, float *accel_y, float *accel_z, float *gyro_x, float *gyro_y, float *gyro_z, float *temperature) {
    uint8_t buf[14];
    // Read 14 bytes starting from ACCEL_XOUT_H
    uint8_t reg = ACCEL_XOUT_H;
    i2c_write_blocking(I2C_PORT, 0x68, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, 0x68, buf, 14, false);
    int16_t ax, ay, az, gx, gy, gz, t;

    // Combine the bytes into signed integers
    ax = (buf[0] << 8) | buf[1];
    ay = (buf[2] << 8) | buf[3];
    az = (buf[4] << 8) | buf[5];
    t = (buf[6] << 8) | buf[7];
    gx = (buf[8] << 8) | buf[9];
    gy = (buf[10] << 8) | buf[11];
    gz = (buf[12] << 8) | buf[13];

    // Convert the values to appropriate units
    float accel_x_g = (float) ax * 0.000061f; // g
    float accel_y_g = (float) ay * 0.000061f; // g
    float accel_z_g = (float) az * 0.000061f; // g
    float temperature_c = (float) (t / 340.00f) + 36.53f; // degrees Celsius

    // Store the results in the provided pointers
    *accel_x = accel_x_g;
    *accel_y = accel_y_g;
    *accel_z = accel_z_g;
    *temperature = temperature_c;

    printf("Accel X: %.2f g, Accel Y: %.2f g, Accel Z: %.2f g\n", accel_x_g, accel_y_g, accel_z_g);
    printf("Temperature: %.2f C\n", temperature_c);
}

void draw_accel_line(float x, float y) {
    // Scale factors to make the line visible but not too long
    if (y < -0.75f) {
        draw_message(60,10,"|");
        draw_message(60,5,"^");
    } else if ((y < -0.5f) && (y > -0.75f)) {
        draw_message(60,10,"^");
    } else if (y > 0.75f) {
        draw_message(60,20,"|");
        draw_message(60,25,"v");
    } else if ((y > 0.5f) && (y < 0.75f)) {
        draw_message(60,20,"v");
    } else {
        draw_message(60,0," ");
        draw_message(60,10," ");    
        draw_message(60,20," ");
        draw_message(60,30," ");  
    }

    if (x < -0.75f) {
        draw_message(65,15,"-->");
    } else if ((x < -0.5f) && (x > -0.75f)) {
        draw_message(65,15,"->");
    } else if (x > 0.75f) {
        draw_message(40,15,"<--");
    } else if ((x > 0.5f) && (x < 0.75f)) {
        draw_message(50,15,"<-");
    }else {
        draw_message(30,15,"                        ");
    }
    
}
