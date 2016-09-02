/*
 * Pano Controller Navigator Master Configuration File
 */

// Address of I2C OLED display. If screen looks scaled edit Adafruit_SSD1306.h
// and pick SSD1306_128_64 or SSD1306_128_32 that matches display type.
#define DISPLAY_I2C_ADDRESS 0x3C
#define OLED_RESET 12
#define TEXT_SIZE 1
#define DISPLAY_ROWS SSD1306_LCDHEIGHT/8/TEXT_SIZE
#define DISPLAY_COLS SSD1306_LCDWIDTH/6/TEXT_SIZE

// board-specific pin settings
#if defined(ARDUINO_ARCH_SAMD) && defined(ARDUINO_SAMD_FEATHER_M0)
#include "config_feather_m0.h"

#elif defined(__arm__) && defined(CORE_TEENSY)
#include "config_teensy.h"

#elif defined(__AVR__)
#error "AVR is not supported"

#else
#error "Unsupported board"
#endif
