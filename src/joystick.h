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

#include "hid.h"

class Joystick : public HID {
private:
    static const int sensitivity = 7; // lower is higher :)
protected:
    int x_pin, y_pin, sw_pin, cancel_pin;
    int x_state = 0, y_state = 0, sw_state = 0, cancel_state = 0;
    int last_read;
    bool connected = false;
public:
    static const int range = (1024/2) >> sensitivity;
    int autorepeat_delay;
    int autorepeat_repeat;
    unsigned read(void) override;
    Joystick(int x_pin, int y_pin, int sw_pin, int cancel_pin);
    ~Joystick(void);
    int getPositionX(bool if_connected=true);
    int getPositionY(bool if_connected=true);
    bool getButtonState(void);
    bool isConnected(void);
};

#endif /* JOYSTICK_H_ */
