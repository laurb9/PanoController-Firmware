/*
 * Camera control
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef CAMERA_H_
#define CAMERA_H_

#include <math.h>

// shutter pulse, in ms - 100 = 0.1s
#define SHUTTER_PULSE 100

class Camera {
protected:
    int focus_pin;
    int shutter_pin;
    unsigned lens_idx;
public:
    int aspect = 23;
    static const unsigned focal_lengths[];
    Camera(int focus_pin, int shutter_pin);
    void shutter(int delay_ms, bool long_pulse);
    unsigned setFocalLength(unsigned focal_length);
    unsigned getFocalLength(void);
    float getHorizFOV(void);
    float getVertFOV(void);
    void setAspect(int aspect);
    float calcFOV(float d){
        // https://en.wikipedia.org/wiki/Angle_of_view
        return 360*atan(d/2/focal_lengths[lens_idx])/M_PI;
    }
};

#endif /* CAMERA_H_ */
