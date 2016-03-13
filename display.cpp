/*
 * display.cpp - Display class with printf() and human-readable units
 * Tiny Scope for Arduino project
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "display.h"

#define CHR_HEIGHT 8 // found in Adafruit_GFX::write(c)
#define CHR_WIDTH 6

Display::Display(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS)
    :Adafruit_SSD1306(SID, SCLK, DC, RST, CS)
{};
Display::Display(int8_t DC, int8_t RST, int8_t CS)
    :Adafruit_SSD1306(DC, RST, CS)
{};
Display::Display(int8_t RST)
    :Adafruit_SSD1306(RST)
{};

void Display::clearDisplay(){
    Adafruit_SSD1306::clearDisplay();
    Adafruit_SSD1306::setCursor(0,0);
};

// from http://playground.arduino.cc/Main/Printf
#include <stdarg.h>
#define PRINTF_BUF 22 // everything we ever print should fit in here.
int Display::printf(const char *format, ...){
    char buf[PRINTF_BUF];
    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);
    Display::print(buf);
    return strlen(buf);
};

#ifdef F // F() macro is available
int Display::printf(const __FlashStringHelper *format, ...){
    char buf[PRINTF_BUF];
    va_list ap;
    va_start(ap, format);
#ifdef __AVR__
    vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
#else
    vsnprintf(buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
#endif
    va_end(ap);
    Display::print(buf);
    return strlen(buf);
};
#endif

/*
 * Helper function to move cursor at text positions
 */
void Display::setTextCursor(uint8_t row, uint8_t col){
    Display::setCursor(col*CHR_WIDTH, row*CHR_HEIGHT);
}
