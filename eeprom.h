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

class {
// fake EEPROM
public:
    int read(int addr){ return 0; };
    void write(int addr, unsigned char c){ };
    int get(int addr, int default_value){ return default_value; };
    void put(int addr, int value){ };
} EEPROM;
#endif

#endif /* EEPROM_H_ */
