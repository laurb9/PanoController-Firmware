/*
 * Pano engine
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "pano.h"

Pano::Pano(Motor& horiz_motor, Motor& vert_motor, Camera& camera, MPU& mpu, int motors_pin)
:horiz_motor(horiz_motor),
 vert_motor(vert_motor),
 camera(camera),
 mpu(mpu),
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
void Pano::setShutter(unsigned speed, unsigned pre_delay, unsigned post_wait, bool long_pulse){
    shutter_delay = speed;
    pre_shutter_delay = pre_delay;
    post_shutter_delay = post_wait;
    shutter_long_pulse = long_pulse;
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
int Pano::getCurRow(void){
    return position / horiz_count;
}
int Pano::getCurCol(void){
    return position % horiz_count;
}
/*
 * Calculate time left to complete pano.
 */
unsigned Pano::getTimeLeft(void){
    int photos = getHorizShots() * getVertShots() - position + 1;
    int steady_delay = 130;
    int seconds = photos * shots_per_position * (pre_shutter_delay + steady_delay + shots_per_position * (shutter_delay + post_shutter_delay)) / 1000 +
        // time needed to move the platform
        // each photo requires a horizontal move (except last one in each row)
        (photos - photos/horiz_count) * camera.getHorizFOV() * horiz_gear_ratio * 60 / DYNAMIC_RPM(HORIZ_MOTOR_RPM, camera.getHorizFOV()) / 360 +
        // row-to-row movement
        photos / horiz_count * camera.getVertFOV() * vert_gear_ratio * 60 / DYNAMIC_RPM(VERT_MOTOR_RPM, camera.getVertFOV()) / 360 +
        // row return horizontal movement
        photos / horiz_count * horiz_fov * 60 / HORIZ_MOTOR_RPM / 360;
    return seconds;
}
/*
 * Helper to calculate grid fit with overlap
 * @param total_size: entire grid size (1-360 degrees)
 * @param overlap: min required overlap in percent (1-99)
 * @param block_size: ref to initial (max) block size (will be updated)
 * @param count: ref to image count (will be updated)
 */
void Pano::gridFit(int total_size, int overlap, float& block_size, int& count){
    if (block_size <= total_size){
        /*
         * For 360 pano, we need to cover entire circle plus overlap.
         * For smaller panos, we cover the requested size only.
         */
        if (total_size != 360){
            total_size = ceil(total_size - block_size * overlap/100);
        }
        block_size = block_size * (100-overlap) / 100;
        count = ceil(total_size / block_size);
        block_size = float(total_size) / count;
    } else {
        count = 1;
    }
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
    motorsEnable(true);
    horiz_motor.setRPM(HORIZ_MOTOR_RPM);
    vert_motor.setRPM(VERT_MOTOR_RPM);
    // set start position
    setMotorsHomePosition();
    position = 0;
}

void Pano::shutter(void){
    delay(pre_shutter_delay);
    mpu.zeroMotionWait(STEADY_TARGET(camera.getVertFOV(), shutter_delay, CAMERA_RESOLUTION), STEADY_TIMEOUT);
    for (unsigned i=shots_per_position; i; i--){
        camera.shutter(shutter_delay, shutter_long_pulse);
        delay(post_shutter_delay);
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
    int cur_row = getCurRow();
    int cur_col = getCurCol();

    if (cur_row >= vert_count ||
        new_row >= vert_count ||
        new_col >= horiz_count ||
        new_col < 0 ||
        new_row < 0){
        // beyond last/first row or column, cannot move there.
        return false;
    }

    if (cur_col != new_col){
        // horizontal adjustment needed
        // figure out shortest path around the circle
        // good idea if on batteries, bad idea when power cable in use
        int move = (new_col-cur_col) * horiz_move;
        if (abs(move) > 180){
            if (move < 0){
                move = 360 + move;
            } else {
                move = move - 360;
            }
        }
        moveMotorsAdaptive(move, 0);
    }
    if (cur_row != new_row){
        // vertical adjustment needed
        moveMotorsAdaptive(0, -(new_row-cur_row)*vert_move);
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

/*
 * Remember current position as "home"
 * (start tracking platform movement to be able to return to it)
 */
void Pano::setMotorsHomePosition(void){
    horiz_home_offset = 0;
    vert_home_offset = 0;
}

/*
 * Move head requested number of degrees at an adaptive speed
 */
void Pano::moveMotorsAdaptive(float h, float v){
    if (h){
        horiz_motor.setRPM(DYNAMIC_HORIZ_RPM(h));
    }
    if (v){
        vert_motor.setRPM(DYNAMIC_VERT_RPM(v));
    }
    moveMotors(h, v);
}
/*
 * Move head requested number of degrees, fixed predefined speed
 */
void Pano::moveMotors(float h, float v){
    if (h){
        horiz_motor.rotate(h * horiz_gear_ratio);
        horiz_home_offset += h;
    }
    if (v){
        vert_motor.rotate(v * vert_gear_ratio);
        vert_home_offset += v;
    }
}

/*
 * Move head back to home position
 */
void Pano::moveMotorsHome(void){
    moveMotorsAdaptive(-horiz_home_offset, -vert_home_offset);
}

void Pano::motorsEnable(bool on){
    digitalWrite(motors_pin, (on) ? HIGH : LOW);
    delay(1);

}
