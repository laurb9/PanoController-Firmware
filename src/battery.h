/*
 * Battery status
 *
 * Copyright (C)2016-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef BATTERY_H_
#define BATTERY_H_


class Battery {
protected:
    int battery_pin, r1, r2, vcc;
public:
    Battery(int battery_pin, int r1, int r2, int vcc);
    void begin(void);
    /*
     * Return battery voltage, in mV
     */
    int voltage(void);
    /*
     * Return battery state as a percentage
     */
    int percent(void);
};

#endif /* BATTERY_H_ */
