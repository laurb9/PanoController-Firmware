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
#include "display.h"
#include "hid.h"
#include "pano_settings.h"

#define DISPLAY_DEVICE Display&

// inactivity time to turn display off (ms)
#define DISPLAY_SLEEP 5*60*1000

enum ClassID { CLASS_SINGLE, CLASS_RANGE, CLASS_LIST, CLASS_NAMES, CLASS_MENU };

/*
 * Do not use directly.
 * Abstract base class for all menu options.
 *
 * Class hierarchy
 * BaseMenu
 *     ActionItem
 *     MultiSelect
 *           RangeSelector
 *           ListSelector
 *                NamedListSelector
 *           Menu
 */
class BaseMenu {
protected:
    int (*onselect)(int) = NULL;    // callback when option selected
    BaseMenu(const char *description, int(*onselect)(int));

public:
    virtual const ClassID getClassID(void) = 0;
    const char* description = NULL;

    virtual void cancel(void);
    virtual void select(void);
    virtual void sync(void);
    virtual int render(DISPLAY_DEVICE display, int rows);
};

class MultiSelect : public BaseMenu {
protected:
    volatile int *value = NULL;     // store current option value here
    int default_val = 0;            // default option value
    int eeprom = 0;                 // EEPROM save location (0 = no save)
    int pointer = 0;                // currently selected entry
    int pos = 0;                    // currently highlighted entry
    int count = 0;                  // number of entries
    int calc_start(int rows);       // helper method

    MultiSelect(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int));

public:
    bool active = false;            // flag indicating the menu is active
    virtual void cancel(void) override;
    virtual void next(void);
    virtual void prev(void);
    virtual void select(void) override;
    virtual void sync(void) override;
    int render(DISPLAY_DEVICE display, int rows);
};

/*
 * Chooses a single option (i.e. "do something")
 */
class ActionItem : public BaseMenu {
public:
    static const ClassID class_id = CLASS_SINGLE;
    const ClassID getClassID(void) override { return class_id; };
    ActionItem(const char *description, int(*onselect)(int));
};

/*
 * Selects a value from a given range, with a predefined step
 */
class RangeSelector : public MultiSelect {
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

/*
 * Selects a numeric value from a list
 */
class ListSelector : public MultiSelect {
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

/*
 * Selects a numeric value from list by its name (needs a corresponding list of names)
 */
class NamedListSelector : public ListSelector {
public:
    static const ClassID class_id = CLASS_NAMES;
    virtual const ClassID getClassID(void){ return class_id; };
    const char* const *names;
    NamedListSelector(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int),
                      int count, const char * const names[], const int values[]);
    int render(DISPLAY_DEVICE display, int rows) override;
};

/*
 * (Sub)Menu handler, selects from a bunch of MultiSelect items
 */
class Menu : public MultiSelect {
protected:
    MultiSelect* active_submenu = NULL;   // active submenu
public:
    static const ClassID class_id = CLASS_MENU;
    virtual const ClassID getClassID(void){ return class_id; };
    BaseMenu* const *menus;
    Menu(const char *description, int count, BaseMenu* const *menus);
    void cancel(void) override;
    void next(void) override;
    void prev(void) override;
    void select(void) override;
    void sync(void) override;
    int render(DISPLAY_DEVICE display, int rows) override;
};

Menu* getMainMenu(PanoSettings& settings);

void displayMenu(Menu& menu, DISPLAY_DEVICE display, const int rows,
                 AllHID& hid, void(*onMenuLoop)(void)=NULL);

#endif /* MENU_H_ */
