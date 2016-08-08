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
    virtual unsigned read(void) = 0;
    virtual int clear(int timeout){
        while (read() && timeout){
            delay(20);
            timeout -= 20;
        }
        return timeout;
    }
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

/*
 * Input mixer, aggregates events from all input sources
 */
class AllHID : public HID {
protected:
    HID* const *inputs;
    int count;
    unsigned last_event;
public:
    AllHID(const int count, HID* const *inputs)
    :inputs(inputs),
     count(count)
    {};
    unsigned read(void) override {
        last_event = EVENT_NONE;
        for (int i=0; i < count; i++){
            last_event |= inputs[i]->read();
        };
        return last_event;
    };
    bool isLastEventUp(void){
        return isEventUp(last_event);
    };
    bool isLastEventDown(void){
        return isEventDown(last_event);
    };
    bool isLastEventLeft(void){
        return isEventLeft(last_event);
    };
    bool isLastEventRight(void){
        return isEventRight(last_event);
    };
    bool isLastEventOk(void){
        return isEventOk(last_event);
    };
    bool isLastEventCancel(void){
        return isEventCancel(last_event);
    };
    void waitAnyKey(int read_timeout=30000, const int clear_timeout=4000){
        while (!read() && read_timeout > 0){
            delay(50);
            read_timeout -= 50;
        }
        clear(clear_timeout);
    }
};

#endif /* HID_H_ */
