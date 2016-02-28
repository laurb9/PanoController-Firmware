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

class Camera {
protected:
    int focus_pin;
    int shutter_pin;
    unsigned lens_idx;
    static const unsigned vert_degrees[];
    static const unsigned horiz_degrees[];
    static const unsigned vert_steps[];
    static const unsigned horiz_steps[];
public:
    int aspect = 23;
    static const unsigned focal_lengths[];
    Camera(int focus_pin, int shutter_pin);
    void shutter(int delay_ms);
    unsigned setFocalLength(unsigned focal_length);;
    unsigned getHorizFOV(void);
    unsigned getVertFOV(void);
    void setAspect(int aspect);
};

#endif /* CAMERA_H_ */
