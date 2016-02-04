/*
 * Pano engine
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "pano.h"

Pano::Pano(Motor horiz_motor, Motor vert_motor, Camera camera,
           int horiz_motor_enable_pin, int vert_motor_enable_pin)
:horiz_motor(horiz_motor),
 vert_motor(vert_motor),
 camera(camera),
 horiz_motor_enable_pin(horiz_motor_enable_pin),
 vert_motor_enable_pin(vert_motor_enable_pin)
{
    pinMode(horiz_motor_enable_pin, OUTPUT);
    pinMode(vert_motor_enable_pin, OUTPUT);
    motorsOn();

    horiz_motor.setMicrostep(1);
    horiz_motor.setRPM(180);
    vert_motor.setMicrostep(1);
    vert_motor.setRPM(60);

    setFOV(360,180);
}
void Pano::setFOV(unsigned horiz_angle, unsigned vert_angle){
    if (horiz_angle && vert_angle && horiz_angle <= 360 && vert_angle <= 180){
        horiz_fov = horiz_angle;
        vert_fov = vert_angle;
    }
}
void Pano::setFocalLength(unsigned focal_length){
    focal_length = camera.setFocalLength(focal_length);
}
void Pano::setShutter(unsigned speed, unsigned pre_delay){
    shutter_delay = speed;
    pre_shutter_delay = pre_delay;
}
void Pano::setShots(unsigned shots){
    shots_per_position = shots;
}
void Pano::setMode(unsigned mode){

}
unsigned Pano::getHorizShots(void){

}
unsigned Pano::getVertShots(void){

}
void Pano::start(void){
    motorsOn();
    // move to start position
    horiz_position = 0;
    vert_position = 0;
    position = 0;
}
bool Pano::next(void){
    motorsOff(); // temporary
    delay(pre_shutter_delay);
    for (unsigned i=shots_per_position; i; i--){
        camera.shutter(shutter_delay);
    }
    motorsOn(); // temporary
    ++position;

    horiz_position += camera.getHorizFOV();
    if (horiz_position < horiz_fov){
        horiz_motor.rotate((int)camera.getHorizFOV());
    } else {
        // move to next row, reset column
        horiz_motor.rotate(-(int)horiz_position);
        horiz_position = 0;
        vert_position += camera.getVertFOV();
        if (vert_position >= vert_fov){
            return false;
        }
        vert_motor.rotate((int)camera.getVertFOV());
    }
    motorsOff(); // temporary
    return true;
}
void Pano::end(void){
    // move to home position
    motorsOff();
}
void Pano::run(void){
    start();
    while(next());
    end();
}
void Pano::motorsOn(void){
    digitalWrite(horiz_motor_enable_pin, LOW);
    digitalWrite(vert_motor_enable_pin, LOW);
}
void Pano::motorsOff(void){
    digitalWrite(horiz_motor_enable_pin, HIGH);
    digitalWrite(vert_motor_enable_pin, HIGH);
}
