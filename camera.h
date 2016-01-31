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
    int lens_idx;
    static const int vert_degrees[];
    static const int horiz_degrees[];
public:
    static const int focal_lengths[];
    Camera(int focus_pin, int shutter_pin);
    void shutter(int delay_ms);
    int setFocalLength(int focal_length);;
};

#endif /* CAMERA_H_ */
