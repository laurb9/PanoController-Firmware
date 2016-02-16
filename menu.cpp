/*
 * Menu
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "menu.h"

#define BLACK 0
#define WHITE 1
#define INVERSE 2

/*
 * OptionSelector: share functionality among the other menu classes
 */
OptionSelector::OptionSelector(const char *description, volatile int *value, int default_val)
:description(description), value(value), default_val(default_val)
{
    pos = default_val;
    if (value){
        *value = default_val;
    }
}

void OptionSelector::render(DISPLAY_DEVICE display, int rows){
    Serial.println(description);
    Serial.println("---------------------");
    display->println(description);
    display->print("---------------------");
}

void OptionSelector::cancel(void){
    pointer = pos;
}

void OptionSelector::open(void){
    pointer = pos;
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

RangeSelector::RangeSelector(const char *description, volatile int *value, int default_val, int min_val, int max_val, int step)
:OptionSelector(description, value, default_val),
 min_val(min_val), max_val(max_val), step(step)
{

};

void RangeSelector::cancel(void){
    pointer = pos;
}

void RangeSelector::open(void){
    pointer = pos;
}

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
    pos = pointer;
    *value = pointer;
}

void RangeSelector::render(DISPLAY_DEVICE display, int rows){
    OptionSelector::render(display, rows);
    const char *marker;

    marker = (pointer < max_val) ? " \x1e": "";
    Serial.println(marker);
    display->println(marker);

    if (pointer == pos) display->setTextColor(BLACK, WHITE);

    Serial.println(pointer);
    display->println(pointer);

    if (pointer == pos) display->setTextColor(WHITE, BLACK);

    marker = (pointer > min_val) ? " \x1f": "";
    Serial.println(marker);
    display->println(marker);
}

/*
 * ListSelector: list of numeric options
 */
ListSelector::ListSelector(const char *description, volatile int *value, int default_val, int count, const int values[])
:OptionSelector(description, value, default_val),
 values(values)
{
    this->count = count;
    // find the position corresponding to the default value
    for (pos=count-1; pos > 0; pos--){
        if (values[pos] == default_val){
            break;
        }
    }
    pointer = pos;
};

void ListSelector::render(DISPLAY_DEVICE display, int rows){
    char buf[16];
    int start = calc_start(rows);

    OptionSelector::render(display, rows);

    for (int i=start; i<start+rows && i<count; i++){
        snprintf(buf, sizeof(buf), "%d", values[i]);

        char marker = (i==pos) ? '\x10' : ' ';

        Serial.print((i==pointer) ? F(">") : F(" "));

        Serial.print(marker);
        display->print(marker);

        if (i == pointer) display->setTextColor(BLACK, WHITE);
        Serial.print(buf);
        display->print(buf);
        if (i == pointer) display->setTextColor(WHITE, BLACK);

        marker = (i==pos) ? '\x11' : ' ';
        Serial.println(marker);
        display->println(marker);
    }
}

/*
 * NamedListSelector: list of named numeric options
 */
NamedListSelector::NamedListSelector(const char *description, volatile int *value, int default_val, int count, const char *names[], const int values[])
:ListSelector(description, value, default_val, count, values),
 names(names)
{
}

void NamedListSelector::render(DISPLAY_DEVICE display, int rows){
    int start = calc_start(rows);

    OptionSelector::render(display, rows);

    for (int i=start; i<start+rows && i<count; i++){
        char marker = (i==pos) ? '\x10' : ' ';

        Serial.print((i==pointer) ? F(">") : F(" "));

        Serial.print(marker);
        display->print(marker);

        if (i == pointer) display->setTextColor(BLACK, WHITE);
        Serial.print(names[i]);
        display->print(names[i]);
        if (i == pointer) display->setTextColor(WHITE, BLACK);

        marker = (i==pos) ? '\x11' : ' ';
        Serial.println(marker);
        display->println(marker);
    }
}

/*
 * Menu: this is a regular menu, nothing is set here.
 */
Menu::Menu(const char *description, int count, const union MenuItem *menus, const int *types)
:OptionSelector(description, NULL, 0),
 menus(menus),
 types(types)
{
    pos = 0;
    this->count = count;
    drilldown = false;
}

void Menu::open(void){
    drilldown = false;
}

void Menu::cancel(void){
    if (drilldown){
        invoke_method(cancel);
        drilldown = false;
    }
}
void Menu::next(void){
    if (drilldown){
        invoke_method(next);
    } else {
        OptionSelector::next();
    }
}
void Menu::prev(void){
    if (drilldown){
        invoke_method(prev);
    } else {
        OptionSelector::prev();
    }
}
void Menu::select(void){
    if (drilldown){
        invoke_method(select);
    } else {
        OptionSelector::select();
        drilldown = true;
        invoke_method(open);
    }
}
void Menu::render(DISPLAY_DEVICE display, int rows){
    int start = calc_start(rows);

    if (drilldown){
        invoke_method(render, display, rows);
        return;
    }

    OptionSelector::render(display, rows);

    for (int i=start; i<start+rows && i<count; i++){
        if (i == pointer){
            Serial.print(F(">"));
            display->setTextColor(BLACK, WHITE);
        }
        Serial.println(menus[i].option->description);
        display->println(menus[i].option->description);
        if (i == pointer){
            display->setTextColor(WHITE, BLACK);
        }
    }
}
