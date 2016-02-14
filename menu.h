/*
 * Menu
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef MENU_H_
#define MENU_H_

#include <Arduino.h>
//#define char __FlashStringHelper

class Options {
public:
    const char* description;
    volatile int *value;
    int default_val;
    Options(const char *description, volatile int *value, int default_val);
    int getCurrentValue(void);
};

class NumericSelection : public Options {
public:
    int min_val, max_val, step;
    NumericSelection(const char *description, volatile int *value, int default_val, int min_val, int max_val, int step);
    int getCurrentValue(void);
};

class ValueOptionMenu : public Options {
public:
    const int count;
    const int *values;
    unsigned pos;
    ValueOptionMenu(const char *description, volatile int *value, int default_val, int count, const int values[]);
    int getCurrentValue(void);
};

class NamedOptionMenu : public ValueOptionMenu {
public:
    const char* *names;
    NamedOptionMenu(const char *description, volatile int *value, int default_val, int count, const char *names[], const int values[]);
    const char *getCurrentName(void);
    int getCurrentValue(void);
};

extern Options menu[];
extern const int menu_size;

#endif /* MENU_H_ */
