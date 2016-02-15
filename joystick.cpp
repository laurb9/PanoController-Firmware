/*
 * Joystick driver
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include <Arduino.h>
#include "joystick.h"

Joystick::Joystick(int sw_pin, int x_pin, int y_pin)
:sw_pin(sw_pin),
 x_pin(x_pin),
 y_pin(y_pin)
{
    pinMode(sw_pin, INPUT_PULLUP);
    //pinMode(x_pin, INPUT);
    //pinMode(y_pin, INPUT);
    sw_state = HIGH;
    x_state = mid_level;
    y_state = mid_level;
    read();
}

unsigned Joystick::read(void){
    unsigned event = EVENT_NONE;
    int current_state;

    if (millis() - last_read < 100){
        return EVENT_NONE;
    }


    // read click switch
    current_state = digitalRead(sw_pin);
    if (current_state != sw_state){
        sw_state = current_state;
        if (sw_state == LOW){
            event |= EVENT_CLICK;
        }
    }

    // read X position
    current_state = (analogRead(x_pin)+(1<<(sensitivity-1))) >> sensitivity;
    if (current_state > x_state && current_state > mid_level){
        event |= EVENT_RIGHT;
    } else if (current_state < x_state && current_state < mid_level){
        event |= EVENT_LEFT;
    }
    x_state = current_state;

    // read Y position
    current_state = (analogRead(y_pin)+(1<<(sensitivity-1))) >> sensitivity;
    if (current_state > y_state && current_state > mid_level){
        event |= EVENT_DOWN;
    } else if (current_state < y_state && current_state < mid_level){
        event |= EVENT_UP;
    }
    y_state = current_state;

    last_read = millis();
    return event;
}
