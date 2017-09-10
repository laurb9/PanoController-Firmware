/*
 * Pano engine
 *
 * Copyright (C)2016,2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef PANO_H_
#define PANO_H_
#include <MultiDriver.h>
#include "camera.h"
#include "mpu.h"
#include "pano_settings.h"

// Calculate maximum allowed movement at given focal length and shutter
// =angular velocity that would cause a pixel to overlap next one within shot time
#define STEADY_TARGET(fov, shutter, resolution) (100*1000/shutter*fov/resolution)

class PanoSetup {
protected:
    Camera& camera;
    unsigned shots_per_position = 1;
    unsigned shutter_delay = 1000/250;
    unsigned pre_shutter_delay = 0;
    unsigned post_shutter_delay = 100;
    bool shutter_long_pulse = false;

    void gridFit(int total_size, int overlap, float& block_size, int& count);

    // Number of horizontal shots to cover horizontal FOV
    int horiz_count;
    // Number of vertical shots to cover vertical FOV
    int vert_count;
public:
    // how long to wait for steady camera before giving up (ms)
    const unsigned STEADY_TIMEOUT = 10000;
    const unsigned CAMERA_RESOLUTION = 4000;

    /*
     * Platform physical parameters
     */
    const unsigned short HORIZ_GEAR_RATIO = 5; // 1:5
    const unsigned short VERT_GEAR_RATIO = 15; // 1:15

    // position overlap, in percent
    const unsigned short MIN_OVERLAP = 25;

    // Current photo position
    unsigned position = 0;
    // How many degrees to move horizontally to advance to next column
    float horiz_move;
    // How many degrees to move vertically to advance to next row
    float vert_move;
    // Preset panorama horizontal field of view
    int horiz_fov;
    // Preset panorama vertical field of view
    int vert_fov;
    unsigned steady_delay_avg = 0;
    PanoSetup(Camera& camera);
    void setFOV(int horiz_angle, int vert_angle);
    void setShutter(unsigned shutter_delay, unsigned pre_delay, unsigned post_wait, bool long_pulse);
    void setShots(unsigned shots);
    void setMode(unsigned mode);
    void computeGrid(void);
    unsigned getHorizShots(void);
    unsigned getVertShots(void);

    // pano info
    int getCurRow(void);
    int getCurCol(void);
};

class Pano : public PanoSetup {
protected:
    MultiDriver& motors;
    MPU& mpu;
    // state information
public:

    float horiz_home_offset = 0;
    float vert_home_offset = 0;

    unsigned steady_delay_avg = 100;

    Pano(MultiDriver& motors, Camera& camera, MPU& mpu);
    void begin(void);

    // pano info
    unsigned getTimeLeft(void);

    // pano execution
    void start(void);
    bool next(void);
    bool prev(void);
    void end(void);
    void run(void);
    void shutter(void);
    void motorsEnable(bool on);

    // positioning within pano
    bool moveTo(int new_position);
    bool moveTo(int new_row, int new_col);

    // pre-positioning (fine grained)
    void setMotorsHomePosition(void);
    void setZeroElevation(void);
    void moveMotorsHome(void);
    void moveMotors(float h, float v);

    // async operation
    unsigned long pollEvent(void);
};

#endif /* PANO_H_ */
