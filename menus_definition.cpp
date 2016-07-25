/*
 * Menu Definition
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "menu.h"

// callbacks
int onStart(int);
int onRepeat(int);
int on360(int);
int onPanoInfo(int);
int onTestShutter(int);

// variables updated directly by menus
extern volatile int horiz;
extern volatile int vert;
extern volatile int focal;
extern volatile int shutter;
extern volatile int pre_shutter;
extern volatile int post_wait;
extern volatile int long_pulse;
extern volatile int shots;
extern volatile int aspect;
extern volatile int motors_enable;
extern volatile int display_invert;

#define EEPROM_ADDR(index) (index * sizeof(int))
#define NO_EEPROM 0

Menu menu("Main Menu", 4, new const PROGMEM BaseMenu* const[4] {

    new Menu("Pano", 6, new const PROGMEM BaseMenu* const[6]{
        new OptionSelector("New Pano", NULL, 0, NO_EEPROM, onStart),
        new OptionSelector("Repeat Last", NULL, 0, NO_EEPROM, onRepeat),
        new OptionSelector("360 Pano", NULL, 0, NO_EEPROM, on360),
        new OptionSelector("Last Pano Info", NULL, 0, NO_EEPROM, onPanoInfo),
        new RangeSelector("Manual Horiz FOV", &horiz, 120, EEPROM_ADDR(1), NULL, 10, 360, 10),
        new RangeSelector("Manual Vert FOV", &vert, 90, EEPROM_ADDR(2), NULL, 10, 180, 10)
    }),

   new Menu("Camera", 8, new const PROGMEM BaseMenu* const[8] {
        new ListSelector("Focal Len", &focal, 35, EEPROM_ADDR(3), NULL, 16,
            (const PROGMEM int[]){12, 14, 16, 20, 24, 28, 35, 50, 75, 105, 200, 300, 400, 450, 500, 600}),
        new NamedListSelector("Shutter", &shutter, 10, EEPROM_ADDR(4), NULL, 11,
            (const PROGMEM char * const[]){"1/100", "1/50", "1/25", "1/10", "1/4", "0.5s", "1s", "2s", "4s", "8s", "MANUAL"},
            (const PROGMEM int[]){10, 20, 40, 100, 250, 500, 1000, 2000, 4000, 8000, 0}),
        new NamedListSelector("Delay", &pre_shutter, 100, EEPROM_ADDR(5), NULL, 6,
            (const PROGMEM char * const[]){"0.1s", "0.5s", "1s", "2s", "4s", "8s"},
            (const PROGMEM int[]){100, 500, 1000, 2000, 4000, 8000}),
        new NamedListSelector("Processing Wait", &post_wait, 100, EEPROM_ADDR(6), NULL, 7,
            (const PROGMEM char * const[]){"0.1s", "0.25s", "0.5s", "1s", "2s", "4s", "8s"},
            (const PROGMEM int[]){100, 250, 500, 1000, 2000, 4000, 8000}),
        new NamedListSelector("Shutter Mode", &long_pulse, 0, EEPROM_ADDR(7), NULL, 2,
            (const PROGMEM char * const[]){"Normal", "Cont Bkt"},
            (const PROGMEM int[]){0, 1}),
        new RangeSelector("Shots #", &shots, 1, EEPROM_ADDR(8), NULL, 1, 5, 1),
        new NamedListSelector("Aspect", &aspect, 23, EEPROM_ADDR(9), NULL, 2,
            (const PROGMEM char * const[]){"P 2:3", "L 3:2"},
            (const PROGMEM int[]){23, 32}),
        new OptionSelector("Test Shutter", NULL, 0, NO_EEPROM, onTestShutter)
    }),

    new NamedListSelector("Motors", &motors_enable, 0, NO_EEPROM, NULL, 2,
        (const PROGMEM char * const[]){"On", "Off"},
        (const PROGMEM int[]){1, 0}),

    new NamedListSelector("Display", &display_invert, 0, NO_EEPROM, NULL, 2,
        (const PROGMEM char * const[]){"Dark", "Bright"},
        (const PROGMEM int[]){0, 1})
});
