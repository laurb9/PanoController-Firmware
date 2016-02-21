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
    return horiz_move ? horiz_fov / horiz_move : 0;
}
unsigned Pano::getVertShots(void){
    return vert_move ? vert_fov / vert_move : 0;
}
/*
 * Helper to calculate grid fit with overlap
 * @param total_size: entire grid size
 * @param block_size: max block size
 * @param overlap: min required overlap in percent (1-99)
 * @returns: new block size (<= block_size) to fit with minimum overlap
 */
int Pano::gridFit(int total_size, int block_size, int overlap){
    int count = 1;
    Serial.print("total_size="); Serial.println(total_size);
    Serial.print("block size="); Serial.println(block_size);

    if (block_size <= total_size){
        count = (total_size * 100/block_size + overlap + 99) / 100;
        block_size = (total_size - block_size + count - 2) / (count - 1);
    }

    Serial.print("new block_size="); Serial.println(block_size);
    Serial.print("count="); Serial.println(count);
    return block_size;
}
/*
 * Calculate shot-to-shot horizontal/vertical head movement,
 * taking overlap into account
 * Must be called every time focal distance or panorama dimensions change.
 */
void Pano::computeGrid(void){
    horiz_move = gridFit(horiz_fov, camera.getHorizFOV(), MIN_OVERLAP);
    vert_move = gridFit(vert_fov, camera.getVertFOV(), MIN_OVERLAP);
}
void Pano::start(void){
    computeGrid();
    motorsOn();
    horiz_motor.setRPM(180);
    vert_motor.setRPM(180);
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

    Serial.print(F("at ")); Serial.print(position);
    Serial.print(F("  horiz_pos=")); Serial.print(horiz_position);
    Serial.print(F("  vert_pos=")); Serial.print(vert_position);
    if (horiz_fov - horiz_position > camera.getHorizFOV()){
        horiz_position += horiz_move;
        Serial.println(F("  H-->"));
        Serial.println(horiz_move*horiz_gear_ratio);
        horiz_motor.rotate(horiz_move*horiz_gear_ratio);
    } else {
        Serial.println(F("  <--H"));
        // move to next row, reset column
        horiz_motor.rotate(-horiz_position*horiz_gear_ratio);
        horiz_position = 0;
        if (vert_fov - vert_position <= camera.getVertFOV()){
            vert_motor.rotate(-vert_position*vert_gear_ratio);
            return false;
        }
        Serial.println(F("  V-->"));
        vert_position += vert_move;
        vert_motor.rotate(vert_move*vert_gear_ratio);
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
