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

#define EVENT_NONE  0
#define EVENT_UP    0b00001
#define EVENT_DOWN  0b00010
#define EVENT_LEFT  0b00100
#define EVENT_RIGHT 0b01000
#define EVENT_CLICK 0b10000

#define isEventUp(event) (event & EVENT_UP)
#define isEventDown(event) (event & EVENT_DOWN)
#define isEventLeft(event) (event & EVENT_LEFT)
#define isEventRight(event) (event & EVENT_RIGHT)
#define isEventClick(event) (event & EVENT_CLICK)

class Joystick {
private:
    static const int sensitivity = 8; // lower is higher :)
    static const int mid_level = 512 >> 8;
protected:
    int sw_pin, x_pin, y_pin;
    int sw_state = 0, x_state, y_state;
    int last_read;
public:
    Joystick(int sw_pin, int x_pin, int y_pin);
    unsigned read(void);
};

#endif /* JOYSTICK_H_ */
