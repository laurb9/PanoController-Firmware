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
    int horiz_gear_ratio = 5; // 1:5
    int vert_gear_ratio = 15; // 1:15
    int horiz_fov;
    int vert_fov;
    int horiz_move;
    int vert_move;
    unsigned shots_per_position = 1;
    unsigned shutter_delay = 1000/250;
    unsigned pre_shutter_delay = 0;
    int gridFit(int total_size, int block_size, int overlap);
    void computeGrid(void);
    // state information
public:
    int horiz_position = 0;
    int vert_position = 0;
    unsigned position;
    volatile unsigned active;
    Pano(Motor& horiz_motor, Motor& vert_motor, Camera& camera, int, int);
    void setFOV(int horiz_angle, int vert_angle);
    void setFocalLength(unsigned focal_length);
    void setShutter(unsigned shutter_delay, unsigned pre_delay);
    void setShots(unsigned shots);
    void setMode(unsigned mode);
    unsigned getHorizShots(void);
    unsigned getVertShots(void);
    void start(void);
    bool next(void);
    void end(void);
    void run(void);
    void motorsOn(void);
    void motorsOff(void);

};

#endif /* PANO_H_ */
