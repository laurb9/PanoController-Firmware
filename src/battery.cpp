/*
 * Battery status
 *
 * Copyright (C)2016-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "battery.h"
#include <Arduino.h>

Battery::Battery(int battery_pin, int r1, int r2, int vcc)
:battery_pin(battery_pin), r1(r1), r2(r2), vcc(vcc)
{}

void Battery::begin(void){
    // Configure input pin and ADC for reading battery voltage
    pinMode(battery_pin, INPUT);
#if defined(__MK20DX256__) || defined(__MKL26Z64__)
    analogReadRes(10);
    analogReadAveraging(32);
#endif
}

/*
 * Return battery voltage, in mV
 */
int Battery::voltage(void){
    return map(analogRead(battery_pin), 0, (1<<10)-1, 0, (vcc * (r1 + r2) / r1)/100) * 100;
}

/*
 * Return battery state as a percentage
 */
int Battery::percent(void){
   int mv = voltage();
   if (mv < 7000) {
       return 10;
   } else {
       return 90;
   }
}
