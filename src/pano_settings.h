/*
 * Pano engine
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef PANO_SETTINGS_H_
#define PANO_SETTINGS_H_

// This does not really belong here
typedef struct {
    volatile int focal = 35,
                 shutter = 100,
                 pre_shutter = 100,
                 post_wait = 500,
                 long_pulse = 0,
                 aspect = 1,
                 shots = 1,
                 motors_enable = 0,
                 display_invert = 0,
                 horiz = 360,
                 vert = 160,
                 running = 0;
} PanoSettings;

#endif /* PANO_SETTINGS_H_ */
