/*
 * Menu Configuration
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "menu.h"

volatile int orientation, aspect, focal, shutter, pre_shutter, shots, horiz, vert, motor;
//#define char __FlashStringHelper
#define NAMES (const char* [])
#define VALUES (const int[])

static NamedOptionMenu menu1("Camera Orientation", &orientation, 1, 3,
           NAMES  {"portrait", "landscape", "square"},
           VALUES {0,          1,           2}
);
static NamedOptionMenu menu2("Aspect Ratio", &aspect, 1, 4,
           NAMES  {"3:2", "16:9", "4:3", "1:1"},
           VALUES {0,     1,      2,     3}
);
static ValueOptionMenu menu3("Focal Length", &focal, 35, 12,
           VALUES {12, 14, 16, 20, 24, 35, 50, 70, 105, 200, 300, 400}
);
static NamedOptionMenu menu4("Shutter Speed", &shutter, 10, 9,
           NAMES  {"1/100", "1/20", "1/10", "1/4","0.5","1",   "2",  "4",  "8"},
           VALUES {10,      50,     100,    250,   500,   1000, 2000, 4000, 8000}
);
static NamedOptionMenu menu5("Pre-Shutter Delay", &pre_shutter, 10, 6,
           NAMES  {"0.1", "0.5", "1",  "2",  "4",  "8"},
           VALUES {100,   500,   1000, 2000, 4000, 8000}
);
static ValueOptionMenu menu6("Shots per position", &shots, 1, 5,
           VALUES {1, 2, 3, 4, 5}
);
static NumericSelection menu7("Horizontal", &horiz, 120, 10, 360, 5);
static NumericSelection menu8("Vertical",   &vert,   45, 10,  90, 5);
static NamedOptionMenu menu9("Motors", &motor, 0, 2,
           NAMES  {"On", "Off"},
           VALUES {0,    1}
);

const Options menu[] = {menu1, menu2, menu3, menu4, menu5, menu6, menu7, menu8, menu9};
const unsigned menu_size = sizeof(menu);
