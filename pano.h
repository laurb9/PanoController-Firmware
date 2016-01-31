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
#include <DRV8834.h>
#include "camera.h"
#include "pano.h"

#define Motor DRV8834

class Pano {
protected:
    Camera camera;
    Motor horiz_motor;
    Motor vert_motor;
public:
    Pano(Motor horiz_motor, Motor vert_motor, Camera camera);
};

#endif /* PANO_H_ */
