/*
 * Pano engine
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef PANO_H_
#define PANO_H_
#include <BasicStepperDriver.h>
#include "camera.h"
#include "pano.h"

#define Motor BasicStepperDriver

#define MIN_OVERLAP 20

class Pano {
protected:
    Motor& horiz_motor;
    Motor& vert_motor;
    Camera& camera;
    int horiz_motor_enable_pin;
    int vert_motor_enable_pin;
    int horiz_move;
    int vert_move;
    int horiz_count;
    int vert_count;
    unsigned shots_per_position = 1;
    unsigned shutter_delay = 1000/250;
    unsigned pre_shutter_delay = 0;
    void gridFit(int total_size, int overlap, int& block_size, int& count);
    // state information
public:
    const int horiz_gear_ratio = 5; // 1:5
    const int vert_gear_ratio = 15; // 1:15
    int horiz_fov;
    int vert_fov;
    unsigned position;
    volatile unsigned active;
    Pano(Motor& horiz_motor, Motor& vert_motor, Camera& camera, int, int);
    void setFOV(int horiz_angle, int vert_angle);
    void setShutter(unsigned shutter_delay, unsigned pre_delay);
    void setShots(unsigned shots);
    void setMode(unsigned mode);
    void computeGrid(void);
    unsigned getHorizShots(void);
    unsigned getVertShots(void);
    bool moveTo(int new_position);
    bool moveTo(int new_row, int new_col);
    void start(void);
    bool next(void);
    bool prev(void);
    void end(void);
    void run(void);
    void shutter(void);
    void motorsOn(void);
    void motorsOff(void);

};

#endif /* PANO_H_ */
