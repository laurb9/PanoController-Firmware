/*
 * Pano engine
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "pano.h"

Pano::Pano(Motor& horiz_motor, Motor& vert_motor, Camera& camera, int motors_pin)
:horiz_motor(horiz_motor),
 vert_motor(vert_motor),
 camera(camera),
 motors_pin(motors_pin)
{
    pinMode(motors_pin, OUTPUT);
    motorsEnable(false);

    setFOV(360,180);
}
void Pano::setFOV(int horiz_angle, int vert_angle){
    if (horiz_angle && vert_angle && horiz_angle <= 360 && vert_angle <= 180){
        horiz_fov = horiz_angle;
        vert_fov = vert_angle;
    }
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
 * Calculate time left to complete pano.
 */
unsigned Pano::getTimeLeft(void){
    int photos = getHorizShots() * getVertShots() - position + 1;
    int seconds = photos * shots_per_position * (pre_shutter_delay + shutter_delay) / 1000 +
        // time needed to move the platform
        photos * camera.getHorizFOV() * horiz_gear_ratio * 60 / HORIZ_MOTOR_RPM / 360 +
        photos / horiz_count * camera.getVertFOV() * vert_gear_ratio * 60 / VERT_MOTOR_RPM / 360;
    return seconds;
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
    horiz_move = camera.getHorizFOV() * horiz_gear_ratio;
    gridFit(horiz_fov * horiz_gear_ratio, MIN_OVERLAP, horiz_move, horiz_count);
    vert_move = camera.getVertFOV() * vert_gear_ratio;
    gridFit(vert_fov * vert_gear_ratio, MIN_OVERLAP, vert_move, vert_count);
}
void Pano::start(void){
    computeGrid();
    motorsEnable(true);
    horiz_motor.setRPM(HORIZ_MOTOR_RPM);
    vert_motor.setRPM(VERT_MOTOR_RPM);
    // move to start position
    position = 0;
}
void Pano::shutter(void){
    delay(pre_shutter_delay);
    for (unsigned i=shots_per_position; i; i--){
        camera.shutter(shutter_delay);
    }
}
/*
 * Move to grid position by photo index (0-number of photos)
 */
bool Pano::moveTo(int new_position){
    int new_row = new_position / horiz_count;
    int new_col = new_position % horiz_count;
    return moveTo(new_row, new_col);
}
/*
 * Move to specified grid position
 * @param new_row: requested row position [0 - vert_count)
 * @param new_col: requested col position [0 - horiz_count)
 */
bool Pano::moveTo(int new_row, int new_col){
    int cur_row = position / horiz_count;
    int cur_col = position % horiz_count;

    if (cur_row >= vert_count || new_row >= vert_count || new_col >= horiz_count){
        // beyond last row or column, cannot move there.
        return false;
    }

    if (cur_col != new_col){
        // horizontal adjustment needed
        // figure out shortest path around the circle
        // good idea if on batteries, bad idea when power cable in use
        int move = (new_col-cur_col)*horiz_move;
        if (abs(move) > 180 * horiz_gear_ratio){
            if (move < 0){
                move = 360 * horiz_gear_ratio + move;
            } else {
                move = move - 360 * horiz_gear_ratio;
            }
        }
        horiz_motor.rotate(move);
    }
    if (cur_row != new_row){
        // vertical adjustment needed
        vert_motor.rotate(-(new_row-cur_row)*vert_move);
    }

    position = new_row * horiz_count + new_col;
    return true;
}
bool Pano::next(void){
    return moveTo(position+1);
}
bool Pano::prev(void){
    return (position > 0) ? moveTo(position-1) : false;
}
void Pano::end(void){
    // move to home position
    moveTo(0, 0);
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
void Pano::motorsEnable(bool on){
    digitalWrite(motors_pin, (on) ? HIGH : LOW);
    delay(1);
}
