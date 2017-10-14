/*
 * G-Code command interpreter
 *
 * Copyright (C)2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef GCODE_H_
#define GCODE_H_

#include <MultiDriver.h>
#include "camera.h"
#include "mpu.h"
#include "battery.h"

enum Coords {ABSOLUTE, RELATIVE};
enum Move {RAPID, LINEAR};
enum Speed {CONSTANT, ACCEL};
typedef struct {
    float a, c;
} Position;

// Modal groups (mutually exclusive, repeat each line)
// https://www.tormach.com/modal_groups_table.html
// Modal Group 1 - Motion
enum Motion {NO_MOTION, G0, G1, G2, G3, G80, G81, G82, G83, G84, G85, G86, G87, G88, G89};
// Modal Group 2 - Plane Selection
enum Plane {G17, G18, G19};
// Modal Group 3 - Distance Mode
enum Distance {G90, G91};
// Modal GRoup 4 - Stopping
enum Wait {M0, M1, M2, M30, M60, M116, M226, M240}; // M116,M226,M240 added by me
// Modal Group 6 - Units
enum Units {G20, G21};
// Non-Modal Group 0
enum NonModal {
    NONE=0,
    G4=1,
    G10=2,
    G28=4,
    G30=8,
    G53=16,
    G92=32,
    G92_1=64,
    G92_2=128,
    G92_3=256,
    M114=512,
    M115=1024,
    M117=2048,
    M503=4096,
};

typedef struct {
    Position current = {0, 0};
    Position target = {0, 0};
    Position origin = {0, 0};
    int delay_ms = 0;
    int lineno = 0;
    Motion motion = NO_MOTION;
    Plane plane = G17;
    Distance distance = G90;
    Wait wait = M1;
    Units units = G20;
    Coords coords = Coords::ABSOLUTE;
    Move move = Move::RAPID;
    Speed speed = Speed::CONSTANT;
    // non-modal commands as a bitfield.
    unsigned nonmodal = 0;
} Command;

class GCode {
protected:
    MultiDriver& motors;
    Camera& camera;
    MPU& mpu;
    Battery& battery;

    Command cmd;
public:
    GCode(MultiDriver& motors, Camera& camera, MPU& mpu, Battery& battery)
        :motors(motors),camera(camera),mpu(mpu),battery(battery){};
    void execute(char buffer[], const char* eob);
    void move(Motion motion, Coords coords, Position& current, Position& target);
};

#endif /* GCODE_H_ */
