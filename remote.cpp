/*
 * IR Remote interface
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "remote.h"

Remote::Remote(int pin)
:pin(pin)
{
    remote = new IRrecv(pin);
    remote->blink13(false);
    remote->enableIRIn();
    last_event = EVENT_NONE;
    next_repeat_time = 0;
}

int Remote::clear(int timeout){
    timeout = HID::clear(timeout);
    last_event = EVENT_NONE;
    return timeout;
}

unsigned Remote::read(void){
    unsigned event = EVENT_NONE;
    if (remote->decode(&remote_buffer)){
        long code = remote_buffer.value;
        remote->resume();
        switch (code){
        case 0xFFFFFFFF: // repeat
            if (millis() > next_repeat_time){
                event = last_event;
            }
            break;
        case 0xFF18E7: // 2
            event |= EVENT_UP;
            break;
        case 0xFF4AB5: // 8
            event |= EVENT_DOWN;
            break;
        case 0xFF10EF: // 4
            event |= EVENT_LEFT;
            break;
        case 0xFF5AA5: // 6
            event |= EVENT_RIGHT;
            break;
        case 0xFFC23D: // PLAY
        case 0xFF38C7: // 5
            event |= EVENT_OK;
            break;
        case 0xFF22DD: // PREV
        case 0xFF6897: // 0
            event |= EVENT_CANCEL;
            break;
        default:
            // To configure new remotes, look for this message in serial console
            Serial.print("Unknown remote code ");
            Serial.println(code, HEX);
        };
        // detect and ignore a too-fast repeat of same key
        if (event){
            if (last_event == event && millis() < next_repeat_time){
                event = EVENT_NONE;
            } else {
                next_repeat_time = millis() + REPEAT_DELAY;
            }
        }
    }
    return event;
}
