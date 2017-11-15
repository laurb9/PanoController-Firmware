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

// (min) shutter pulse, in ms - 10 = 0.01s
// should give camera enough time to recognize the pulse
#define MIN_SHUTTER_PULSE 10

class Camera {
protected:
    int focus_pin;
    int shutter_pin;
    unsigned focal_length;
    float horiz_fov, vert_fov;
    float calcFOV(float d){
        // https://en.wikipedia.org/wiki/Angle_of_view
        return 360*atan(d/2/focal_length)/M_PI;
    }
    void calcAllFOV(void);
public:
    int aspect = 23;
    Camera(int focus_pin, int shutter_pin);
    bool isShutterConnected(void);
    bool isFocusConnected(void);
    void shutter(int delay_ms, bool long_pulse);
    unsigned setFocalLength(unsigned focal_length);
    unsigned getFocalLength(void){ return focal_length; };
    float getHorizFOV(void){ return horiz_fov; };
    float getVertFOV(void){ return vert_fov; };
    void setAspect(int aspect);
};

#endif /* CAMERA_H_ */
