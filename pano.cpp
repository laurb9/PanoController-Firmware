/*
 * Pano engine
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "pano.h"

Pano::Pano(Motor horiz_motor, Motor vert_motor, Camera camera)
:horiz_motor(horiz_motor),
 vert_motor(vert_motor),
 camera(camera)
{
    horiz_motor.setMicrostep(1);
    horiz_motor.setRPM(180);
    vert_motor.setMicrostep(1);
    vert_motor.setRPM(60);
}

