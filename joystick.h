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
protected:
    int sw_pin, x_pin, y_pin;
public:
    Joystick(int sw_pin, int x_pin, int y_pin);
};

#endif /* JOYSTICK_H_ */
