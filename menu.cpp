/*
 * Menu
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "menu.h"
#define WHITE 0
#define BLACK 1
#define INVERSE 2

Options::Options(const char *description, volatile int *value, int default_val)
:description(description), value(value), default_val(default_val)
{
    *value = default_val;
}

int Options::getCurrentValue(void){
    return *value;
}

void Options::render(Print *display, int rows){
    Serial.println();
    Serial.println(description);
    Serial.println("---------------------");
    display->println(description);
    display->print("---------------------");
}

NumericSelection::NumericSelection(const char *description, volatile int *value, int default_val, int min_val, int max_val, int step)
:Options(description, value, default_val),
 min_val(min_val), max_val(max_val), step(step)
{

};

void NumericSelection::render(Print *display, int rows){
    Options::render(display, rows);
}

ValueOptionMenu::ValueOptionMenu(const char *description, volatile int *value, int default_val, int count, const int values[])
:Options(description, value, default_val),
 count(count), values(values)
{
    for (pos=count; pos > 0; pos--){
        if (values[pos] == default_val){
            break;
        }
    }
    pointer = pos;
};

void ValueOptionMenu::render(Print *display, int rows){
    Options::render(display, rows);
}

int NamedOptionMenu::getCurrentValue(void){
    return values[pos];
}

NamedOptionMenu::NamedOptionMenu(const char *description, volatile int *value, int default_val, int count, const char *names[], const int values[])
:ValueOptionMenu(description, value, default_val, count, values),
 names(names)
{
}

void ValueOptionMenu::open(void){
    pointer = pos;
}

void ValueOptionMenu::next(void){
    if (pointer < count-1){
        pointer++;
    }
}
void ValueOptionMenu::prev(void){
    if (pointer > 0){
        pointer--;
    }
}
void ValueOptionMenu::select(void){
    pos = pointer;
}

void NamedOptionMenu::render(Print *display, int rows){
    ValueOptionMenu::render(display, rows);
    int start;
    if (pointer < rows/2){
        start = 0;
    } else if (pointer < count - rows/2){
        start = pointer - rows/2;
    } else {
        start = count - rows;
    }
    for (int i=start; i<start+rows && i<count; i++){
        Serial.print((i==pos) ? "*" : " ");
        display->print((i==pos) ? "*" : " ");
        Serial.print((i==pointer) ? ">" : " ");
        display->print((i==pointer) ? ">" : " ");
        Serial.print(names[i]);
        display->print(names[i]);
        Serial.print((i==pointer) ? "<" : " ");
        display->print((i==pointer) ? "<" : " ");
        Serial.println((i==pos) ? "*" : " ");
        display->println((i==pos) ? "*" : " ");
    }
}

const char* NamedOptionMenu::getCurrentName(void){
    return names[pos];
}
