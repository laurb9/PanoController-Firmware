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

Options::Options(const char *description, volatile int *value, int default_val)
:description(description), value(value), default_val(default_val)
{
    pos = default_val;
    if (value){
        *value = default_val;
    }
}

void Options::render(DISPLAY_DEVICE display, int rows){
    Serial.println();
    Serial.println(description);
    Serial.println("---------------------");
    display->println(description);
    display->print("---------------------");
}

void Options::cancel(void){
    pointer = pos;
}

void Options::open(void){
    pointer = pos;
}

void Options::next(void){
    if (pointer < count-1){
        pointer++;
    }
}
void Options::prev(void){
    if (pointer > 0){
        pointer--;
    }
}
void Options::select(void){
    pos = pointer;
}

int Options::calc_start(int rows){
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

NumericSelection::NumericSelection(const char *description, volatile int *value, int default_val, int min_val, int max_val, int step)
:Options(description, value, default_val),
 min_val(min_val), max_val(max_val), step(step)
{

};

void NumericSelection::cancel(void){
    pointer = pos;
}

void NumericSelection::open(void){
    pointer = pos;
}

void NumericSelection::next(void){
    if (pointer > min_val){
        pointer -= step;
    }
}
void NumericSelection::prev(void){
    if (pointer < max_val){
        pointer += step;
    }
}
void NumericSelection::select(void){
    pos = pointer;
    *value = pointer;
}

void NumericSelection::render(DISPLAY_DEVICE display, int rows){
    Options::render(display, rows);
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

ValueOptionMenu::ValueOptionMenu(const char *description, volatile int *value, int default_val, int count, const int values[])
:Options(description, value, default_val),
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

void ValueOptionMenu::render(DISPLAY_DEVICE display, int rows){
    Options::render(display, rows);
}

NamedOptionMenu::NamedOptionMenu(const char *description, volatile int *value, int default_val, int count, const char *names[], const int values[])
:ValueOptionMenu(description, value, default_val, count, values),
 names(names)
{
}

void NamedOptionMenu::render(DISPLAY_DEVICE display, int rows){
    ValueOptionMenu::render(display, rows);
    int start = calc_start(rows);

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

Menu::Menu(const char *description, int count, const union MenuItem *menus, const int *types)
:Options(description, NULL, 0),
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
        Options::next();
    }
}
void Menu::prev(void){
    if (drilldown){
        invoke_method(prev);
    } else {
        Options::prev();
    }
}
void Menu::select(void){
    if (drilldown){
        invoke_method(select);
    } else {
        Options::select();
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

    Options::render(display, rows);

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
