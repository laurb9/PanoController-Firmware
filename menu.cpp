/*
 * Menu
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "menu.h"

Options::Options(const char *description, volatile int *value, int default_val)
:description(description), value(value), default_val(default_val)
{

}

NumericSelection::NumericSelection(const char *description, volatile int *value, int default_val, int min_val, int max_val, int step)
:Options(description, value, default_val),
 min_val(min_val), max_val(max_val), step(step)
{

};

ValueOptionMenu::ValueOptionMenu(const char *description, volatile int *value, int default_val, int count, const int values[])
:Options(description, value, default_val),
 count(count), values(values)
{

};

NamedOptionMenu::NamedOptionMenu(const char *description, volatile int *value, int default_val, int count, const char *names[], const int values[])
:Options(description, value, default_val),
 count(count), names(names), values(values)
{

}
