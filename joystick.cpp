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
    x_state = 0;
    y_state = 0;
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
            for (int i=50; i && digitalRead(sw_pin) == LOW; i--){
                delay(10);
            };
        }
    }

    // read X position
    current_state = getPositionX();
    if (current_state > x_state && current_state > 0){
        event |= EVENT_RIGHT;
    } else if (current_state < x_state && current_state < 0){
        event |= EVENT_LEFT;
    }
    x_state = current_state;

    // read Y position
    current_state = getPositionY();
    if (current_state > y_state && current_state > 0){
        event |= EVENT_UP;
    } else if (current_state < y_state && current_state < 0){
        event |= EVENT_DOWN;
    }
    y_state = current_state;

    last_read = millis();
    return event;
}

int Joystick::getPositionX(void){
    return (analogRead(x_pin)-512+(1<<(sensitivity-1))) >> sensitivity;
}

int Joystick::getPositionY(void){
    return (512-analogRead(y_pin)+(1<<(sensitivity-1))) >> sensitivity;
}

bool Joystick::getButtonState(void){
    return digitalRead(sw_pin) == LOW;
}
