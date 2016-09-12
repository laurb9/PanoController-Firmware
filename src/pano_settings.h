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

typedef volatile int16_t settings_t;

typedef struct {
    settings_t
    focal = 35,
    shutter = 100,
    pre_shutter = 100,
    post_wait = 500,
    long_pulse = 0,
    aspect = 1,
    shots = 1,
    motors_enable = 0,
    display_invert = 0,
    horiz = 360,
    vert = 160;
} PanoSettings;

typedef struct {
    settings_t
    battery = 0,
    motors_enable = 0,
    display_invert = 0,
    running = 0,
    position = 0,
    steady_delay_avg = 0;
    float horiz_offset = 0;
    float vert_offset = 0;
} PanoState;

#endif /* PANO_SETTINGS_H_ */
