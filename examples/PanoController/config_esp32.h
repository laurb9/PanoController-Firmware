/*
 * Pano Controller Configuration File for ESP32 board
 * https://github.com/espressif/arduino-esp32/blob/master/docs/esp32_pinmap.png
 * https://desire.giesecke.tk/index.php/2018/07/06/reserved-gpios/
 */
#include <Arduino.h>

/* SPI (for display)
SPI2 and SPI3 are general purpose SPI controllers, sometimes referred to as HSPI and VSPI
SDA = IO 23
SCLK = IO 18
D/C = IO 21
RST = IO 22
CS = IO 5
*/

// Camera shutter controls
#define CAMERA_FOCUS GPIO_NUM_16
#define CAMERA_SHUTTER GPIO_NUM_17

// Battery measurement pin R1/R2
#define BATTERY GPIO_NUM_34 //34 # ADC1 CH0

// MPU (accel/gyro)
// GPIO_NUM_21 I2C SDA
// GPIO_NUM_22 I2C SCL
#define MPU_INT GPIO_NUM_2
#define MPU_VCC GPIO_NUM_4

// Future devices
//#define COMPASS_DRDY xx

// Stepper drivers control
#define DIR GPIO_NUM_27
#define VERT_STEP GPIO_NUM_25
#define HORIZ_STEP GPIO_NUM_26

// this should be hooked up to nENABLE on both drivers
#define nENABLE GPIO_NUM_14
