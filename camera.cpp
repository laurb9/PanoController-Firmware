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

// taken from https://en.wikipedia.org/wiki/Angle_of_view for now
const unsigned Camera::focal_lengths[] = {
        12, 14, 16, 20, 24, 35, 50, 70, 105, 200, 300, 400
};
const float Camera::vert_degrees[] = {
        90, 81, 74, 62, 53, 38, 27, 19,  13,   7,   5,   3
};
const float Camera::horiz_degrees[] = {
       111,103, 95, 82, 74, 54, 40, 29,  19,  10,   7,   5
};

Camera::Camera(int focus_pin, int shutter_pin)
:focus_pin(focus_pin),
 shutter_pin(shutter_pin)
{
    pinMode(focus_pin, OUTPUT);
    pinMode(shutter_pin, OUTPUT);
    digitalWrite(focus_pin, HIGH);
    digitalWrite(shutter_pin, HIGH);
    setFocalLength(24);
}

void Camera::shutter(int delay_ms){
    digitalWrite(focus_pin, LOW);
    digitalWrite(shutter_pin, LOW);
    delay(delay_ms);
    digitalWrite(focus_pin, HIGH);
    digitalWrite(shutter_pin, HIGH);
}

unsigned Camera::setFocalLength(unsigned focal_length){
    unsigned i;
    for (i=0; i<sizeof(focal_lengths) && focal_lengths[i] < focal_length; i++);
    lens_idx = i < sizeof(focal_lengths) ? i : sizeof(focal_lengths)-1;
    return lens_idx;
}

float Camera::getHorizFOV(void){
    unsigned fov;
    switch (aspect){
    case 32:
        fov = horiz_degrees[lens_idx];
        break;
    case 23:
        fov = vert_degrees[lens_idx];
        break;
    default:
        fov = 0;
    }
    return fov;
}

float Camera::getVertFOV(void){
    unsigned fov;
    switch (aspect){
    case 32:
        fov = vert_degrees[lens_idx];
        break;
    case 23:
        fov = horiz_degrees[lens_idx];
        break;
    default:
        fov = 0;
    }
    return fov;
}

/*
 * Set image aspect ratio
 *
 * @param aspect: number indicating WH, for example 32 for 3:2
 * allowable values are 32 (landscape) and 23 (portrait)
 */
void Camera::setAspect(int aspect){
    this->aspect = aspect;
}
