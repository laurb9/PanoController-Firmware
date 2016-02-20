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
#include <avr/pgmspace.h>
#include <Adafruit_GFX.h>

#define FLASH_STRING (const __FlashStringHelper *)

#define DISPLAY_DEVICE Adafruit_GFX&

enum ClassID { CLASS_OPTIONS, CLASS_RANGE, CLASS_LIST, CLASS_NAMES, CLASS_MENU };

class OptionSelector {
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
    bool active = false;      // flag indicating the option selector is active
    OptionSelector(const char *description, volatile int *value, int default_val);
    void cancel(void);
    void open(void);
    void next(void);
    void prev(void);
    void select(void);
    void sync(void);
    void render(DISPLAY_DEVICE display, int rows);
};

class RangeSelector : public OptionSelector {
public:
    static const ClassID class_id = CLASS_RANGE;
    int min_val, max_val, step;
    RangeSelector(const char *description, volatile int *value, int default_val, int min_val, int max_val, int step);
    void next(void);
    void prev(void);
    void select(void);
    void sync(void);
    void render(DISPLAY_DEVICE display, int rows);
};

class ListSelector : public OptionSelector {
public:
    static const ClassID class_id = CLASS_LIST;
    const int *values;
    ListSelector(const char *description, volatile int *value, int default_val, int count, const int values[]);
    void select(void);
    void sync(void);
    void render(DISPLAY_DEVICE display, int rows);
};

class NamedListSelector : public ListSelector {
public:
    static const ClassID class_id = CLASS_NAMES;
    const char* const *names;
    NamedListSelector(const char *description, volatile int *value, int default_val, int count, const char * const names[], const int values[]);
    void render(DISPLAY_DEVICE display, int rows);
};

class Menu : public OptionSelector {
protected:
    bool drilldown = false;   // flag indicating that selected menu option is active
public:
    static const ClassID class_id = CLASS_MENU;
    const union MenuItem *menus;
    const int *types;
    Menu(const char *description, int count, const union MenuItem * const menus, const int *types);
    void open(void);
    void cancel(void);
    void next(void);
    void prev(void);
    void select(void);
    void sync(void);
    void render(DISPLAY_DEVICE display, int rows);
};

union MenuItem {
    OptionSelector *option;
    RangeSelector *slider;
    ListSelector *values;
    NamedListSelector *names;
    Menu *menu;
};

/*
 * Macro to cast a MenuItem to the correct pointer type and invoke the requested method
 */
#define invoke_method(pos, method, ...) \
switch(types[pos]){ \
case Menu::class_id: menus[pos].menu->method(__VA_ARGS__); break; \
case NamedListSelector::class_id: menus[pos].names->method(__VA_ARGS__); break; \
case ListSelector::class_id: menus[pos].values->method(__VA_ARGS__); break; \
case RangeSelector::class_id: menus[pos].slider->method(__VA_ARGS__); break; \
case OptionSelector::class_id: menus[pos].option->method(__VA_ARGS__); break; \
default: break; \
        }

extern Menu menu;

#endif /* MENU_H_ */
