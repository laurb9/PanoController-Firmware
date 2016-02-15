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

#define TYPE_OPTIONS 1
#define TYPE_NUMERIC 2
#define TYPE_VALUES  3
#define TYPE_NAMES   4

class Options {
public:
    int type = TYPE_OPTIONS;
    const char* description;
    volatile int *value;
    int default_val;
    Options(const char *description, volatile int *value, int default_val);
    int getCurrentValue(void);
    void render(Print *display, int rows, int pointer);
};

class NumericSelection : public Options {
public:
    int type = TYPE_NUMERIC;
    int min_val, max_val, step;
    NumericSelection(const char *description, volatile int *value, int default_val, int min_val, int max_val, int step);
    int getCurrentValue(void);
    void render(Print *display, int rows, int pointer);
};

class ValueOptionMenu : public Options {
public:
    int type = TYPE_VALUES;
    const int count;
    const int *values;
    int pos;
    ValueOptionMenu(const char *description, volatile int *value, int default_val, int count, const int values[]);
    int getCurrentValue(void);
    void render(Print *display, int rows, int pointer);
};

class NamedOptionMenu : public ValueOptionMenu {
public:
    int type = TYPE_NAMES;
    const char* *names;
    NamedOptionMenu(const char *description, volatile int *value, int default_val, int count, const char *names[], const int values[]);
    const char* getCurrentName(void);
    int getCurrentValue(void);
    void render(Print *display, int rows, int pointer);
};

union MenuItem {
    Options *option;
    NumericSelection *slider;
    ValueOptionMenu *values;
    NamedOptionMenu *names;
};

extern union MenuItem menu[];
extern const int menu_size;

#endif /* MENU_H_ */
