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

/*
 * Interrupt handler triggered by joystick button click
 */
volatile static int button_clicked = false;
void button_click(){
    button_clicked = true;
}

Joystick::Joystick(int sw_pin, int x_pin, int y_pin)
:sw_pin(sw_pin),
 x_pin(x_pin),
 y_pin(y_pin)
{
    pinMode(sw_pin, INPUT_PULLUP);
    pinMode(x_pin, INPUT);
    pinMode(y_pin, INPUT);
    sw_state = HIGH;
    x_state = 0;
    y_state = 0;
    read();
    attachInterrupt(digitalPinToInterrupt(sw_pin), button_click, FALLING);
}

Joystick::~Joystick(void){
    // clean up
    detachInterrupt(digitalPinToInterrupt(sw_pin));
}

bool Joystick::isConnected(void){
    // if joystick is not center at start, it is not connected
    if (!connected){
        if (getPositionX(false) == 0 && getPositionY(false) == 0){
            connected || Serial.println("Joystick connected.");
            connected = true;
        }
    }
    return connected;
}

unsigned Joystick::read(void){
    unsigned event = EVENT_NONE;
    int current_state;

    if (millis() - last_read < 100){
        return EVENT_NONE;
    }

    if (!isConnected()){
        return event;
    }

    // read click switch
    current_state = (button_clicked) ? LOW : digitalRead(sw_pin);
    if (current_state != sw_state){
        sw_state = current_state;
        if (sw_state == LOW){
            event |= EVENT_OK;
            for (int i=50; i && digitalRead(sw_pin) == LOW; i--){
                delay(10);
            };
        }
        button_clicked = false;
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

int Joystick::getPositionX(bool if_connected){
    if (if_connected && !isConnected()){
        return 0;
    }
    return ((int)analogRead(x_pin)-512+(1<<(sensitivity-1))) >> sensitivity;
}

int Joystick::getPositionY(bool if_connected){
    if (if_connected && !isConnected()){
        return 0;
    }
    return (512-(int)analogRead(y_pin)+(1<<(sensitivity-1))) >> sensitivity;
}

bool Joystick::getButtonState(void){
    return digitalRead(sw_pin) == LOW;
}
