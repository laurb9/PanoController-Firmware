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

#include <Adafruit_GFX.h>
//#define char __FlashStringHelper

#define DISPLAY_DEVICE Adafruit_GFX*

enum ClassID { CLASS_OPTIONS, CLASS_NUMERIC, CLASS_VALUES, CLASS_NAMES, CLASS_MENU };

class Options {
protected:
    int pointer = 0;
    int pos = 0;
    int calc_start(int rows);
public:
    static const ClassID class_id = CLASS_OPTIONS;
    const char* description;
    volatile int *value;
    int default_val;
    int count = 0;
    Options(const char *description, volatile int *value, int default_val);
    void cancel(void);
    void open(void);
    void next(void);
    void prev(void);
    void select(void);
    void render(DISPLAY_DEVICE display, int rows);
};

class NumericSelection : public Options {
public:
    static const ClassID class_id = CLASS_NUMERIC;
    int min_val, max_val, step;
    NumericSelection(const char *description, volatile int *value, int default_val, int min_val, int max_val, int step);
    void open(void);
    void cancel(void);
    void next(void);
    void prev(void);
    void select(void);
    void render(DISPLAY_DEVICE display, int rows);
};

class ValueOptionMenu : public Options {
public:
    static const ClassID class_id = CLASS_VALUES;
    const int *values;
    ValueOptionMenu(const char *description, volatile int *value, int default_val, int count, const int values[]);
    void render(DISPLAY_DEVICE display, int rows);
};

class NamedOptionMenu : public ValueOptionMenu {
public:
    static const ClassID class_id = CLASS_NAMES;
    const char* *names;
    NamedOptionMenu(const char *description, volatile int *value, int default_val, int count, const char *names[], const int values[]);
    void render(DISPLAY_DEVICE display, int rows);
};

class Menu : public Options {
protected:
    bool drilldown = false;
public:
    static const ClassID class_id = CLASS_MENU;
    const union MenuItem *menus;
    const int *types;
    Menu(const char *description, int count, const union MenuItem *menus, const int *types);
    void open(void);
    void cancel(void);
    void next(void);
    void prev(void);
    void select(void);
    void render(DISPLAY_DEVICE display, int rows);
};

union MenuItem {
    Options *option;
    NumericSelection *slider;
    ValueOptionMenu *values;
    NamedOptionMenu *names;
    Menu *menu;
};

/*
 * Macro to cast a MenuItem to the correct pointer type and invoke the requested method
 */
#define invoke_method(method, ...) \
switch(types[pos]){ \
case Menu::class_id: menus[pos].menu->method(__VA_ARGS__); break; \
case NamedOptionMenu::class_id: menus[pos].names->method(__VA_ARGS__); break; \
case ValueOptionMenu::class_id: menus[pos].values->method(__VA_ARGS__); break; \
case NumericSelection::class_id: menus[pos].slider->method(__VA_ARGS__); break; \
case Options::class_id: menus[pos].option->method(__VA_ARGS__); break; \
default: break; \
        }

extern Menu menu;

#endif /* MENU_H_ */
