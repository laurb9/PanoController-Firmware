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

class BaseMenu {
protected:
    BaseMenu(const char *description, volatile int *value, int eeprom, int(*onselect)(int))
    :description(description),
     value(value),
     eeprom(eeprom),
     onselect(onselect)
    {};

public:
    virtual const ClassID getClassID(void) = 0;
    const char* description = NULL;
    volatile int *value = NULL;
    int default_val = 0;
    int eeprom = 0;
    int (*onselect)(int) = NULL;

    virtual void cancel(void) = 0;
    virtual void open(void) = 0;
    virtual void next(void) = 0;
    virtual void prev(void) = 0;
    virtual void select(void) = 0;
    virtual void sync(void) = 0;
    virtual int render(DISPLAY_DEVICE display, int rows) = 0;
};

class OptionSelector : public BaseMenu {
protected:
    int pointer = 0;
    int pos = 0;
    int calc_start(int rows);
public:
    static const ClassID class_id = CLASS_OPTIONS;
    virtual const ClassID getClassID(void){ return class_id; };
    int count = 0;
    bool active = false;      // flag indicating the option selector is active
    OptionSelector(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int));
    void cancel(void) override;
    void open(void) override;
    void next(void) override;
    void prev(void) override;
    void select(void) override;
    void sync(void) override;
    int render(DISPLAY_DEVICE display, int rows) override;
};

class RangeSelector : public OptionSelector {
public:
    static const ClassID class_id = CLASS_RANGE;
    virtual const ClassID getClassID(void){ return class_id; };
    int min_val, max_val, step;
    RangeSelector(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int),
                  int min_val, int max_val, int step);
    void next(void) override;
    void prev(void) override;
    void select(void) override;
    void sync(void) override;
    int render(DISPLAY_DEVICE display, int rows) override;
};

class ListSelector : public OptionSelector {
public:
    static const ClassID class_id = CLASS_LIST;
    virtual const ClassID getClassID(void){ return class_id; };
    const int *values;
    ListSelector(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int),
                 int count, const int values[]);
    void select(void) override;
    void sync(void) override;
    int render(DISPLAY_DEVICE display, int rows) override;
};

class NamedListSelector : public ListSelector {
public:
    static const ClassID class_id = CLASS_NAMES;
    virtual const ClassID getClassID(void){ return class_id; };
    const char* const *names;
    NamedListSelector(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int),
                      int count, const char * const names[], const int values[]);
    int render(DISPLAY_DEVICE display, int rows) override;
};

class Menu : public OptionSelector {
protected:
    bool drilldown = false;   // flag indicating that selected menu option is active
public:
    static const ClassID class_id = CLASS_MENU;
    virtual const ClassID getClassID(void){ return class_id; };
    const BaseMenu* const *menus;
    Menu(const char *description, int count, const BaseMenu* const *menus);
    BaseMenu * getBaseMenuAtPos(const int pos);
    void cancel(void) override;
    void open(void) override;
    void next(void) override;
    void prev(void) override;
    void select(void) override;
    void sync(void) override;
    int render(DISPLAY_DEVICE display, int rows) override;
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
(getBaseMenuAtPos(pos)->getClassID() == Menu::class_id) ? \
    FLASH_CAST_PTR(Menu, menus, pos)->method(__VA_ARGS__) : (\
(getBaseMenuAtPos(pos)->getClassID() == NamedListSelector::class_id) ? \
    FLASH_CAST_PTR(NamedListSelector, menus, pos)->method(__VA_ARGS__) : (\
(getBaseMenuAtPos(pos)->getClassID() == ListSelector::class_id) ? \
    FLASH_CAST_PTR(ListSelector, menus, pos)->method(__VA_ARGS__) : (\
(getBaseMenuAtPos(pos)->getClassID() == RangeSelector::class_id) ? \
    FLASH_CAST_PTR(RangeSelector, menus, pos)->method(__VA_ARGS__) : \
    FLASH_CAST_PTR(OptionSelector, menus, pos)->method(__VA_ARGS__))));


extern Menu menu;

#endif /* MENU_H_ */
