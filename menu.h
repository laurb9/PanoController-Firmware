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
    bool active = false;      // flag indicating the menu is active

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
    BaseMenu* const *menus;
    Menu(const char *description, int count, BaseMenu* const *menus);
    void cancel(void) override;
    void open(void) override;
    void next(void) override;
    void prev(void) override;
    void select(void) override;
    void sync(void) override;
    int render(DISPLAY_DEVICE display, int rows) override;
};

extern Menu menu;

#endif /* MENU_H_ */
