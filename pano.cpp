/*
 * Pano engine
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "pano.h"

Pano::Pano(Motor& horiz_motor, Motor& vert_motor, Camera& camera,
           int horiz_motor_enable_pin, int vert_motor_enable_pin)
:horiz_motor(horiz_motor),
 vert_motor(vert_motor),
 camera(camera),
 horiz_motor_enable_pin(horiz_motor_enable_pin),
 vert_motor_enable_pin(vert_motor_enable_pin)
{
    pinMode(horiz_motor_enable_pin, OUTPUT);
    pinMode(vert_motor_enable_pin, OUTPUT);
    motorsOff();

    horiz_motor.setMicrostep(1);
    vert_motor.setMicrostep(1);

    setFOV(360,180);
}
void Pano::setFOV(int horiz_angle, int vert_angle){
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
    return horiz_count;
}
unsigned Pano::getVertShots(void){
    return vert_count;
}
/*
 * Helper to calculate grid fit with overlap
 * @param total_size: entire grid size
 * @param overlap: min required overlap in percent (1-99)
 * @param block_size: ref to initial block size (will be updated)
 * @param count: ref to image count (will be updated)
 */
void Pano::gridFit(int total_size, int overlap, int& block_size, int& count){
    Serial.print("total_size="); Serial.println(total_size);
    Serial.print("block size="); Serial.println(block_size);

    if (block_size <= total_size){
        count = (100*total_size - overlap*block_size - 1) / ((100 - overlap)*block_size);
        block_size = (total_size - block_size + count - 1) / count;
        count++;
    } else {
        count = 1;
    }

    Serial.print("count="); Serial.println(count);
    Serial.print("new block_size="); Serial.println(block_size);
}
/*
 * Calculate shot-to-shot horizontal/vertical head movement,
 * taking overlap into account
 * Must be called every time focal distance or panorama dimensions change.
 */
void Pano::computeGrid(void){
    horiz_move = camera.getHorizFOV();
    gridFit(horiz_fov, MIN_OVERLAP, horiz_move, horiz_count);
    vert_move = camera.getVertFOV();
    gridFit(vert_fov, MIN_OVERLAP, vert_move, vert_count);
}
void Pano::start(void){
    computeGrid();
    motorsOn();
    horiz_motor.setRPM(60);
    vert_motor.setRPM(180);
    // move to start position
    position = 0;
}
void Pano::shutter(void){
    delay(pre_shutter_delay);
    for (unsigned i=shots_per_position; i; i--){
        camera.shutter(shutter_delay);
    }
}
bool Pano::next(void){
    motorsOn(); // temporary

    ++position;

    Serial.print(F("at ")); Serial.println(position);

    if (position % horiz_count){
        // still within current row
        Serial.println(F("  H-->"));
        horiz_motor.rotate(horiz_move*horiz_gear_ratio);
    } else {
        // end of row. reset column
        Serial.println(F("  <--H"));
        horiz_motor.rotate((1-horiz_count)*horiz_move*horiz_gear_ratio);
        if (position / horiz_count < vert_count){
            // still within pano, change row, reset column
            Serial.println(F("  V-->"));
            vert_motor.rotate(-vert_move*vert_gear_ratio);
        } else {
            Serial.println(F("  <--V"));
            vert_motor.rotate((vert_count-1)*vert_move*vert_gear_ratio);
            return false;
        }
    }
    motorsOff(); // temporary
    return true;
}
void Pano::end(void){
    // move to home position
    motorsOff();
}
/*
 * Execute a full pano run.
 */
void Pano::run(void){
    start();
    do {
        shutter();
    } while(next());
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
