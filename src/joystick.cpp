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
volatile static uint8_t button_clicked = false;
void button_click(){
    button_clicked = true;
}
volatile static uint8_t cancel_clicked = false;
void cancel_click(){
    cancel_clicked = true;
}

Joystick::Joystick(int x_pin, int y_pin, int sw_pin, int cancel_pin)
:x_pin(x_pin),
 y_pin(y_pin),
 sw_pin(sw_pin),
 cancel_pin(cancel_pin)
{
    pinMode(x_pin, INPUT);
    pinMode(y_pin, INPUT);
    pinMode(sw_pin, INPUT_PULLUP);
    pinMode(cancel_pin, INPUT_PULLUP);
    sw_state = HIGH;
    cancel_state = HIGH;
    x_state = 0;
    y_state = 0;
    read();
    attachInterrupt(digitalPinToInterrupt(sw_pin), button_click, FALLING);
    attachInterrupt(digitalPinToInterrupt(cancel_pin), cancel_click, FALLING);
}

Joystick::~Joystick(void){
    // clean up
    detachInterrupt(digitalPinToInterrupt(sw_pin));
    detachInterrupt(digitalPinToInterrupt(cancel_pin));
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

/*
 *
 */
static bool read_button_state(int& state, volatile uint8_t& clicked, int pin){
    int current_state;
    bool active = false;
    current_state = (clicked) ? LOW : digitalRead(pin);
    if (current_state != state){
        state = current_state;
        if (state == LOW){
            active = true;
            for (int i=50; i && digitalRead(pin) == LOW; i--){
                delay(10);
            }
        }
        clicked = false;
    }
    return active;
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

    // read click switches
    if (read_button_state(sw_state, button_clicked, sw_pin)){
        event |= EVENT_OK;
    }
    if (read_button_state(cancel_state, cancel_clicked, cancel_pin)){
        event |= EVENT_CANCEL;
    }

    // read X position
    current_state = getPositionX();
    if (!x_state || millis() > next_repeat_time){
        if (current_state > 0){
            event |= EVENT_RIGHT;
        } else if (current_state < 0){
            event |= EVENT_LEFT;
        }
        x_state = current_state;
        next_repeat_time = millis() + (x_state) ? REPEAT_INTERVAL : REPEAT_DELAY;
    }

    // read Y position
    current_state = getPositionY();
    if (!y_state || millis() > next_repeat_time){
        if (current_state > 0){
            event |= EVENT_UP;
        } else if (current_state < 0){
            event |= EVENT_DOWN;
        }
        y_state = current_state;
        next_repeat_time = millis() + (y_state) ? REPEAT_INTERVAL : REPEAT_DELAY;
    }
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
