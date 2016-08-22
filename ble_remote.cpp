/*
 * Bluetooth LE Remote
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "ble_remote.h"

#define PACKET_BUTTON_LEN               (5)
#define READ_BUFSIZE                    (20)

#define BUTTON_ID(code) (code & 0x0f)
#define IS_PRESSED(code) (code >> 4)

BLERemote::BLERemote(Adafruit_BluefruitLE_SPI& ble)
:ble(ble)
{}

void BLERemote::init(void){
    if (ble.isConnected()){
        if (!active){
            ble.setMode(BLUEFRUIT_MODE_DATA);
            active = true;
            Serial.println("Connected");
        }
    } else {
        active = false;
    }
}

unsigned BLERemote::read(void){
    unsigned event = EVENT_NONE;

    uint8_t buttonCode = readButtonCode();

    if (IS_PRESSED(buttonCode)){
        switch (BUTTON_ID(buttonCode)){
        case 1:
            event |= EVENT_OK;
            break;
        case 3:
            event |= EVENT_CANCEL;
            break;
        case 5:
            event |= EVENT_UP;
            break;
        case 6:
            event |= EVENT_DOWN;
            break;
        case 7:
            event |= EVENT_LEFT;
            break;
        case 8:
            event |= EVENT_RIGHT;
            break;
        }

        next_repeat_time = millis() + REPEAT_DELAY;
        last_event = event;

    } else if (BUTTON_ID(buttonCode)){    // button depressed
        last_event = EVENT_NONE;

    } else if (last_event){            // button currently remaining pressed
        if (millis() > next_repeat_time && !isEventOk(last_event) && !isEventCancel(last_event)){
            event = last_event;
            next_repeat_time = millis() + REPEAT_INTERVAL;
        }
    }
    return event;
}

/*
 * Read the controller messages.
 * Returns a byte which contains button number in low nibble, and pressed state in the high.
 */
uint8_t BLERemote::readButtonCode(void){
    static uint8_t packetbuffer[READ_BUFSIZE];

    init();
    if (!active){
        return 0;
    }

    int i = 0;
    while (ble.available() && i < PACKET_BUTTON_LEN){
        char c =  ble.read();
        if (i == 0 and c != '!'){
            continue;
        }
        packetbuffer[i++] = c;
    }

    if (!i || packetbuffer[1] != 'B'){
        return 0;
    }

    // Verify checksum
    uint8_t checksum = 1;
    for (; i > 0;){
        checksum += packetbuffer[--i];
    }
    if (checksum){ // checksum mismatch
        return 0;
    }

    return (packetbuffer[2] - '0') + (packetbuffer[3] - '0' << 4);
}
