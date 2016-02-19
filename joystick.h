/*
 * Pano engine
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef JOYSTICK_H_
#define JOYSTICK_H_

class Joystick {
private:
    enum Event {
        EVENT_NONE  = 0,
        EVENT_UP    = 0b00001,
        EVENT_DOWN  = 0b00010,
        EVENT_LEFT  = 0b00100,
        EVENT_RIGHT = 0b01000,
        EVENT_CLICK = 0b10000,
    };
    static const int sensitivity = 7; // lower is higher :)
protected:
    int sw_pin, x_pin, y_pin;
    int sw_state = 0, x_state = 0, y_state = 0;
    int last_read;
public:
    Joystick(int sw_pin, int x_pin, int y_pin);
    unsigned read(void);
    static bool isEventUp(unsigned event){
        return (event & EVENT_UP);
    };
    static bool isEventDown(unsigned event){
        return (event & EVENT_DOWN);
    };
    static bool isEventLeft(unsigned event){
        return (event & EVENT_LEFT);
    };
    static bool isEventRight(unsigned event){
        return (event & EVENT_RIGHT);
    };
    static bool isEventClick(unsigned event){
        return (event & EVENT_CLICK);
    };
    int getPositionX(void);
    int getPositionY(void);
    bool getButtonState(void);
};

#endif /* JOYSTICK_H_ */
