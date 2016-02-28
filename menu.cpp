/*
 * Menu
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <EEPROM.h>
#include "menu.h"

#define BLACK 0
#define WHITE 1
#define INVERSE 2

/*
 * OptionSelector: share functionality among the other menu classes
 */
OptionSelector::OptionSelector(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int))
:description(description),
 value(value),
 eeprom(eeprom),
 onselect(onselect)
{
    active = false;
    // override default with eeprom value, if set
    if (eeprom){
        EEPROM.get(eeprom, this->default_val);
    }
    if (this->default_val <= 0){
        this->default_val = default_val;
    }
    if (value){
        *value = this->default_val;
    }
}

int OptionSelector::render(DISPLAY_DEVICE display, int rows){
    Serial.println(FLASH_STRING description);
    Serial.println("---------------------");
    display.println(FLASH_STRING description); rows--;
    if (rows > 4){
        display.print("---------------------"); rows--;
    }
    return rows;
}

void OptionSelector::cancel(void){
    pointer = pos;
    active = false;
}

void OptionSelector::open(void){
    pointer = pos;
    active = true;
}

void OptionSelector::next(void){
    if (pointer < count-1){
        pointer++;
    }
}
void OptionSelector::prev(void){
    if (pointer > 0){
        pointer--;
    }
}
void OptionSelector::select(void){
    pos = pointer;
    OptionSelector::sync();
    if (onselect){
        onselect(*value);
    }
}
void OptionSelector::sync(void){
    if (eeprom){
        EEPROM.put(eeprom, *value);
        Serial.print("EEPROM save "); Serial.print(*value); Serial.print(" for "); Serial.println(description);
    }
}

int OptionSelector::calc_start(int rows){
    int start = 0;
    if (count > rows && pointer > rows/2){
        if (pointer < count - rows/2){
            start = pointer - rows/2;
        } else {
            start = count - rows;
        }
    }
    return start;
}

/*
 * RangeSelector: a number with up/down controls
 */

RangeSelector::RangeSelector(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int),
                             int min_val, int max_val, int step)
:OptionSelector(description, value, default_val, eeprom, onselect),
 min_val(min_val), max_val(max_val), step(step)
{
    pointer = this->default_val;
    pos = pointer;
};

void RangeSelector::next(void){
    if (pointer > min_val){
        pointer -= step;
    }
}
void RangeSelector::prev(void){
    if (pointer < max_val){
        pointer += step;
    }
}
void RangeSelector::select(void){
    *value = pointer;
    OptionSelector::select();
}
void RangeSelector::sync(void){
    pointer = *value;
    pos = pointer;
    OptionSelector::sync();
}

int RangeSelector::render(DISPLAY_DEVICE display, int rows){
    const char *marker;
    rows = OptionSelector::render(display, rows);

    marker = (pointer < max_val) ? " \x1e": "";
    Serial.println(marker);
    display.println(marker); rows--;

    if (pointer == pos) display.setTextColor(BLACK, WHITE);

    Serial.println(pointer);
    display.println(pointer); rows--;

    if (pointer == pos) display.setTextColor(WHITE, BLACK);

    marker = (pointer > min_val) ? " \x1f": "";
    Serial.println(marker);
    display.println(marker); rows--;
    return rows;
}

/*
 * ListSelector: list of numeric options
 */
ListSelector::ListSelector(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int),
                           int count, const int values[])
:OptionSelector(description, value, default_val, eeprom, onselect),
 values(values)
{
    this->count = count;
    // find the position corresponding to the default value
    for (pos=count-1; pos > 0; pos--){
        if (FLASH_READ_INT(values, pos) == this->default_val){
            break;
        }
    }
    pointer = pos;
};

void ListSelector::select(void){
    *value = FLASH_READ_INT(values, pointer);
    OptionSelector::select();
}
void ListSelector::sync(void){
    // find the position corresponding to the default value
    for (pos=count-1; pos > 0; pos--){
        if (FLASH_READ_INT(values,pos) == *value){
            break;
        }
    }
    pointer = pos;
    OptionSelector::sync();
}

int ListSelector::render(DISPLAY_DEVICE display, int rows){
    char buf[16];
    int start;

    rows = OptionSelector::render(display, rows);
    start = calc_start(rows);

    for (int i=start; i<start+rows && i<count; i++){
        snprintf(buf, sizeof(buf), "%d", FLASH_READ_INT(values, i));

        char marker = (i==pointer) ? '\x10' : ' ';

        Serial.print((i==pointer) ? F(">") : F(" "));

        display.print(marker);

        if (i == pos) display.setTextColor(BLACK, WHITE);
        Serial.print(buf);
        display.print(buf);
        if (i == pos) display.setTextColor(WHITE, BLACK);

        marker = (i==pointer) ? '\x11' : ' ';
        display.println(marker);
        Serial.println("");
    }
    return 0;
}

/*
 * NamedListSelector: list of named numeric options
 */
NamedListSelector::NamedListSelector(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int),
                                     int count, const char * const names[], const int values[])
:ListSelector(description, value, default_val, eeprom, onselect, count, values),
 names(names)
{
}

int NamedListSelector::render(DISPLAY_DEVICE display, int rows){
    int start;

    rows = OptionSelector::render(display, rows);
    start = calc_start(rows);

    for (int i=start; i<start+rows && i<count; i++){
        char marker = (i==pointer) ? '\x10' : ' ';

        Serial.print((i==pointer) ? F(">") : F(" "));

        display.print(marker);

        if (i == pos) display.setTextColor(BLACK, WHITE);
        Serial.println(FLASH_READ_STR(names, i));
        display.print(FLASH_READ_STR(names, i));
        if (i == pos) display.setTextColor(WHITE, BLACK);

        marker = (i==pointer) ? '\x11' : ' ';
        display.println(marker);
    }
    return 0;
}

/*
 * Menu: this is a regular menu, nothing is set here.
 */
Menu::Menu(const char *description, int count, const union MenuItem * const menus, const int *types)
:OptionSelector(description, NULL, 0, 0, NULL),
 menus(menus),
 types(types)
{
    pos = 0;
    this->count = count;
    drilldown = false;
}

void Menu::open(void){
    OptionSelector::open();
    drilldown = false;
}

void Menu::cancel(void){
    if (drilldown){
        invoke_method(pos, cancel);
        if (FLASH_READ_INT(types, pos) != Menu::class_id
            || ! FLASH_CAST_PTR(Menu, menus, pos)->active){
            drilldown = false;
        }
    } else {
        OptionSelector::cancel();
    }
}
void Menu::next(void){
    if (drilldown){
        invoke_method(pos, next);
    } else {
        OptionSelector::next();
    }
}
void Menu::prev(void){
    if (drilldown){
        invoke_method(pos, prev);
    } else {
        OptionSelector::prev();
    }
}
void Menu::select(void){
    if (drilldown){
        invoke_method(pos, select);
    } else {
        OptionSelector::select();
        drilldown = true;
        invoke_method(pos, open);
    }
}
void Menu::sync(void){
    for (int i=0; i<count; i++){
        invoke_method(i, sync);
    }
}
int Menu::render(DISPLAY_DEVICE display, int rows){
    int start;

    if (drilldown){
        rows = invoke_method(pos, render, display, rows);
        return rows;
    }

    rows = OptionSelector::render(display, rows);
    start = calc_start(rows);

    for (int i=start; i<start+rows && i<count; i++){
        if (i == pointer){
            Serial.print(F(">"));
            display.setTextColor(BLACK, WHITE);
        }
        Serial.println(FLASH_STRING FLASH_CAST_PTR(OptionSelector, menus, i)->description);
        display.println(FLASH_STRING FLASH_CAST_PTR(OptionSelector, menus, i)->description);
        if (i == pointer){
            display.setTextColor(WHITE, BLACK);
        }
    }
    return 0;
}
