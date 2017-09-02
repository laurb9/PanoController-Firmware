/*
 * Pano engine configuration and settings
 *
 * Copyright (C)2016,2017 Laurentiu Badea
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
    horiz = 360,
    vert = 160,
    motors_on = 0,
    display_invert = 0;
} PanoSettings;

typedef struct  __attribute__ ((packed)) {
    volatile float horiz_offset = 1.234;
    volatile float vert_offset = 56.78;
    volatile int16_t battery = 0;
    volatile int16_t position = 0;
    volatile int16_t steady_delay_avg = 0;
    volatile int8_t motors_on = 0;
    volatile int8_t running = 0;
    volatile int8_t paused = 7;
} PanoState;

#endif /* PANO_SETTINGS_H_ */
