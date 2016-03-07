/*
 * IR Remote interface
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef REMOTE_H_
#define REMOTE_H_

#include <Arduino.h>
#include <IRremote.h>
#include "hid.h"

class Remote : public HID {
private:
    int pin;
    int last_event;
    IRrecv* remote;
    decode_results remote_buffer;
protected:
public:
    Remote(int pin);
    unsigned read(void);
    int clear(int timeout);
};

#endif /* REMOTE_H_ */
