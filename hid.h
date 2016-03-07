/*
 * Human Interface Device
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef HID_H_
#define HID_H_

class HID {
protected:
    enum Event {
        EVENT_NONE  = 0,
        EVENT_UP    = 0b000001,
        EVENT_DOWN  = 0b000010,
        EVENT_LEFT  = 0b000100,
        EVENT_RIGHT = 0b001000,
        EVENT_OK    = 0b010000,
        EVENT_CANCEL= 0b100000
    };
public:
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
    static bool isEventOk(unsigned event){
        return (event & EVENT_OK);
    };
    static bool isEventCancel(unsigned event){
        return (event & EVENT_CANCEL);
    }
};

#endif /* HID_H_ */
