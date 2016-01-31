/*
 * Camera control
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "camera.h"
#include <Arduino.h>

Camera::Camera(int focus_pin, int shutter_pin)
:focus_pin(focus_pin),
 shutter_pin(shutter_pin)
{
    pinMode(focus_pin, OUTPUT);
    pinMode(shutter_pin, OUTPUT);
    digitalWrite(focus_pin, HIGH);
    digitalWrite(shutter_pin, HIGH);
}

void Camera::shutter(int delay_ms){
    digitalWrite(focus_pin, LOW);
    digitalWrite(shutter_pin, LOW);
    delay(delay_ms);
    digitalWrite(focus_pin, HIGH);
    digitalWrite(shutter_pin, HIGH);
}
