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
    int eeprom = 0;
    int count = 0;
    bool active = false;      // flag indicating the option selector is active
    OptionSelector(const char *description, volatile int *value, int default_val, int eeprom);
    void cancel(void);
    void open(void);
    void next(void);
    void prev(void);
    void select(void);
    void sync(void);
    int render(DISPLAY_DEVICE display, int rows);
};

class RangeSelector : public OptionSelector {
public:
    static const ClassID class_id = CLASS_RANGE;
    int min_val, max_val, step;
    RangeSelector(const char *description, volatile int *value, int default_val, int eeprom,
                  int min_val, int max_val, int step);
    void next(void);
    void prev(void);
    void select(void);
    void sync(void);
    int render(DISPLAY_DEVICE display, int rows);
};

class ListSelector : public OptionSelector {
public:
    static const ClassID class_id = CLASS_LIST;
    const int *values;
    ListSelector(const char *description, volatile int *value, int default_val, int eeprom,
                 int count, const int values[]);
    void select(void);
    void sync(void);
    int render(DISPLAY_DEVICE display, int rows);
};

class NamedListSelector : public ListSelector {
public:
    static const ClassID class_id = CLASS_NAMES;
    const char* const *names;
    NamedListSelector(const char *description, volatile int *value, int default_val, int eeprom,
                      int count, const char * const names[], const int values[]);
    int render(DISPLAY_DEVICE display, int rows);
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
    int render(DISPLAY_DEVICE display, int rows);
};

union MenuItem {
    OptionSelector *option;
    RangeSelector *slider;
    ListSelector *values;
    NamedListSelector *names;
    Menu *menu;
};

/* ugliest code ever follows */

#if defined(__AVR__)
    #define FLASH_STRING (const __FlashStringHelper *)
    #define FLASH_READ_INT(ptr, idx) pgm_read_word_near(&(ptr[idx]))
    #define FLASH_CAST_PTR(cls, ptr, idx) ((cls*)FLASH_READ_INT(ptr, idx))
    #define FLASH_READ_STR(ptr, idx) FLASH_STRING FLASH_READ_INT(ptr, idx)
#else
    #define FLASH_STRING
    #define FLASH_READ_INT(ptr, idx) ptr[idx]
    #define FLASH_CAST_PTR(cls, ptr, idx) (((cls**)ptr)[idx])
    #define FLASH_READ_STR(ptr, idx) FLASH_CAST_PTR(char, ptr, idx)
#endif /* __AVR__ */

/*
 * Hack to cast a MenuItem to the correct pointer type and invoke the requested method
 */
#define invoke_method(pos, method, ...) \
(FLASH_READ_INT(types, pos) == Menu::class_id) ? \
    FLASH_CAST_PTR(Menu, menus, pos)->method(__VA_ARGS__) : (\
(FLASH_READ_INT(types, pos) == NamedListSelector::class_id) ? \
    FLASH_CAST_PTR(NamedListSelector, menus, pos)->method(__VA_ARGS__) : (\
(FLASH_READ_INT(types, pos) == ListSelector::class_id) ? \
    FLASH_CAST_PTR(ListSelector, menus, pos)->method(__VA_ARGS__) : (\
(FLASH_READ_INT(types, pos) == RangeSelector::class_id) ? \
    FLASH_CAST_PTR(RangeSelector, menus, pos)->method(__VA_ARGS__) : \
    FLASH_CAST_PTR(OptionSelector, menus, pos)->method(__VA_ARGS__))));


extern Menu menu;

#endif /* MENU_H_ */
