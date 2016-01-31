/*
 * Joystick driver
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "joystick.h"

Joystick::Joystick(int sw_pin, int x_pin, int y_pin)
:sw_pin(sw_pin),
 x_pin(x_pin),
 y_pin(y_pin)
{}

