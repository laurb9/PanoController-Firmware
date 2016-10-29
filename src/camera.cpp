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
    setFocalLength(24);
}

void Camera::shutter(int delay_ms, bool long_pulse){
    int shutter_pulse;
    if (long_pulse){
        // don't allow a delay shorter than MIN_SHUTTER_PULSE
        shutter_pulse = max(delay_ms, MIN_SHUTTER_PULSE);
    } else {
        shutter_pulse = MIN_SHUTTER_PULSE;
    }
    pinMode(focus_pin, OUTPUT);
    pinMode(shutter_pin, OUTPUT);
    digitalWrite(focus_pin, LOW);
    digitalWrite(shutter_pin, LOW);
    /* keep line active LOW for <shutter_pulse> ms */
    delay(shutter_pulse);
    pinMode(focus_pin, INPUT);
    pinMode(shutter_pin, INPUT);
    /* wait to complete the shutter action */
    if (delay_ms > shutter_pulse){
        delay(delay_ms - shutter_pulse);
    }
}

unsigned Camera::setFocalLength(unsigned focal_length){
    this->focal_length = focal_length;
    calcAllFOV();
    return focal_length;
}

unsigned Camera::getFocalLength(void){
    return focal_length;
}

/*
 * Set image aspect ratio
 *
 * @param aspect: number indicating WH, for example 32 for 3:2
 * allowable values are 32 (landscape) and 23 (portrait)
 */
void Camera::setAspect(int aspect){
    this->aspect = aspect;
    calcAllFOV();
}

float Camera::getHorizFOV(void){
    return horiz_fov;
}

float Camera::getVertFOV(void){
    return vert_fov;
}

void Camera::calcAllFOV(void){
    float sensor_horiz, sensor_vert;
    switch (aspect){
    case 23:
        sensor_horiz = 24;
        sensor_vert = 36;
        break;
    case 32:
    default:
        sensor_horiz = 36;
        sensor_vert = 24;
        break;
    }
    horiz_fov = calcFOV(sensor_horiz);
    vert_fov = calcFOV(sensor_vert);
}
