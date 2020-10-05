/*
 * Pano Controller Configuration File for ESP32 board
 * Already wired in https://github.com/Xinyuan-LilyGO/TTGO-T-Display
 * 19 TFT_MOSI
 * 18 TFT_SCLK
 * 5  TFT_CS
 * 16 TFT_DC
 * 4  TFT_BL
 * 21 SDA
 * 22 SCL
 * 34 ADC_IN
 * 35 BUTTON1
 * 0  BUTTON2
 * 14 ADC_POWER
 * 
 * Reserved GPIOs
 * not as inputs: 0, 2, 6-11, 12, 15, 16, 17
 * not as outputs:      6-11,         16, 17  34-39
 * Reference https://desire.giesecke.tk/index.php/2018/07/06/reserved-gpios/
 */
#include <Arduino.h>

// Camera shutter controls
#define CAMERA_FOCUS   GPIO_NUM_37
#define CAMERA_SHUTTER GPIO_NUM_38

// Battery measurement pin R1/R2
#define BATTERY GPIO_NUM_36 // ADC0

// MPU (accel/gyro)
// GPIO_NUM_21 I2C SDA
// GPIO_NUM_22 I2C SCL
#define MPU_INT GPIO_NUM_13
#define MPU_VCC GPIO_NUM_17

// Future devices
//#define COMPASS_DRDY xx

// Stepper drivers control. nENABLE, DIR are connected to both drivers
#define nENABLE    GPIO_NUM_33
#define DIR        GPIO_NUM_27
#define VERT_STEP  GPIO_NUM_25
#define HORIZ_STEP GPIO_NUM_26
