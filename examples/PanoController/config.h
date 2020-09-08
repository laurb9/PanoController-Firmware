/*
 * Pano Controller Master Configuration File
 */

// Address of I2C OLED display. If screen looks scaled edit Adafruit_SSD1306.h
// and pick SSD1306_128_64 or SSD1306_128_32 that matches display type.
#define DISPLAY_I2C_ADDRESS 0x3C
#define OLED_RESET 12
#define TEXT_SIZE 1
#define DISPLAY_ROWS SSD1306_LCDHEIGHT/8/TEXT_SIZE
#define DISPLAY_COLS SSD1306_LCDWIDTH/6/TEXT_SIZE

// Battery monitoring settings
#define VCC 3300
// R1/R2 is the voltage divisor in Ω (GND-R1-A0-R2-Vin)
// measure resistors and enter actual values for a more accurate voltage
#define BATT_R1 9980
#define BATT_R2 46500

// MPU (accel/gyro)
#define MPU_I2C_ADDRESS 0x68

// Stepper motors and drivers configuration
// steps per revolution
#define MOTOR_STEPS 200
// microsteps
#define MICROSTEPS 32
// max RPM 
#define MOTOR_RPM 500
// max acceleration/deceleration in steps/sec^2
#define MOTOR_ACCEL 1000
#define MOTOR_DECEL 1000

// Platform parameters
#define HORIZ_GEAR_RATIO 5   // 1:5
#define VERT_GEAR_RATIO 15   // 1:15

// board-specific pin settings
#if defined(ARDUINO_SAMD_FEATHER_M0)
#include "config_feather_m0.h"

#elif defined(__arm__) && defined(CORE_TEENSY)
#include "config_teensy.h"

#elif defined(__AVR__)
#error "AVR is not supported"

#else
#error "Unsupported board"
#endif
