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
#define BATT_R1 7060
#define BATT_R2 42600

// MPU (accel/gyro)
#define MPU_I2C_ADDRESS 0x68
 
#include "platform_gigapan.h"
#include "platform_panocontroller_v4.h"
#include "platform_mecha_e1.h"
#ifndef MOTOR_STEPS
#warning "Missing PLATFORM_ setting, default to PLATFORM_GIGAPAN"
#define PLATFORM_GIGAPAN
#include "platform_gigapan.h"
#endif

// board-specific pin settings
#if defined(ARDUINO_SAMD_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0)
#include "config_feather_m0.h"

#elif defined(ESP32)
#include "config_esp32.h"

#elif defined(__arm__) && defined(CORE_TEENSY)
#include "config_teensy.h"

#elif defined(__AVR__)
#error "AVR is not supported"

#else
#error "Unsupported board"
#endif
