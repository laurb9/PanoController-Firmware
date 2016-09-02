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
int onAboutPanoController(int);

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

#define NO_EEPROM 0
#define USE_EEPROM assignEEPROM()

/*
 * allocate successive EEPROM addresses (each call returns the next available int-sized cell)
 */
int assignEEPROM(){
    static int location = 0;
    return ++location * sizeof(int);
}

/*
 * Return the main menu object
 */
Menu* getMainMenu(void){
    static Menu* menu =
    new Menu("Main Menu", 5, new BaseMenu* const[5] {

        new Menu("Pano", 6, new BaseMenu* const[6]{
            new ActionItem("New Pano", onStart),
            new ActionItem("Repeat Last", onRepeat),
            new ActionItem("360 Pano", on360),
            new ActionItem("Last Pano Info", onPanoInfo),
            new RangeSelector("Manual Horiz FOV", &horiz, 120, USE_EEPROM, NULL, 10, 360, 10),
            new RangeSelector("Manual Vert FOV", &vert, 90, USE_EEPROM, NULL, 10, 180, 10)
        }),

       new Menu("Camera", 8, new BaseMenu* const[8] {
            new ListSelector("Focal Len", &focal, 35, USE_EEPROM, NULL, 16,
                (const int[]){12, 14, 16, 20, 24, 28, 35, 50, 75, 105, 200, 300, 400, 450, 500, 600}),
            new NamedListSelector("Shutter", &shutter, 10, USE_EEPROM, NULL, 11,
                (const char * const[]){"1/100", "1/50", "1/25", "1/10", "1/4", "0.5s", "1s", "2s", "4s", "8s", "MANUAL"},
                (const int[]){10, 20, 40, 100, 250, 500, 1000, 2000, 4000, 8000, 0}),
            new NamedListSelector("Delay", &pre_shutter, 100, USE_EEPROM, NULL, 6,
                (const char * const[]){"0.1s", "0.5s", "1s", "2s", "4s", "8s"},
                (const int[]){100, 500, 1000, 2000, 4000, 8000}),
            new NamedListSelector("Processing Wait", &post_wait, 100, USE_EEPROM, NULL, 7,
                (const char * const[]){"0.1s", "0.25s", "0.5s", "1s", "2s", "4s", "8s"},
                (const int[]){100, 250, 500, 1000, 2000, 4000, 8000}),
            new NamedListSelector("Shutter Mode", &long_pulse, 0, USE_EEPROM, NULL, 2,
                (const char * const[]){"Normal", "Cont Bkt"},
                (const int[]){0, 1}),
            new RangeSelector("Shots #", &shots, 1, USE_EEPROM, NULL, 1, 5, 1),
            new NamedListSelector("Aspect", &aspect, 23, USE_EEPROM, NULL, 2,
                (const char * const[]){"P 2:3", "L 3:2"},
                (const int[]){23, 32}),
            new ActionItem("Test Shutter", onTestShutter)
        }),

        new NamedListSelector("Motors", &motors_enable, 0, NO_EEPROM, NULL, 2,
            (const char * const[]){"On", "Off"},
            (const int[]){1, 0}),

        new NamedListSelector("Display", &display_invert, 0, NO_EEPROM, NULL, 2,
            (const char * const[]){"Dark", "Bright"},
            (const int[]){0, 1}),

        new ActionItem("About", onAboutPanoController)
    });
    return menu;
};
