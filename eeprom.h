/*
 * Persistent storage (EEPROM) support
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef EEPROM_H_
#define EEPROM_H_

#if !defined(ARDUINO_SAMD_ZERO)
#include <EEPROM.h>
#else

#include <Adafruit_BluefruitLE_SPI.h>

extern Adafruit_BluefruitLE_SPI ble;

class {
// fake EEPROM
public:
    int read(int addr){ return 0; };
    void write(int addr, unsigned char c){ };
    int get(int addr, int& value){
        int32_t v;
        ble.readNVM(addr, &v);
        value = v;
        return value;
    };
    void put(int addr, int value){
        int old_value;
        get(addr, old_value);
        if (old_value != value){
            ble.writeNVM(addr, value);
        }
    };
} EEPROM;
#endif

#endif /* EEPROM_H_ */
