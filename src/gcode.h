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
enum Wait {M0, M1, M2, M30, M60, M226}; // M116,M226,M240 added by me
// Modal Group 6 - Units
enum Units {G20, G21};
// Non-Modal Group 0
enum NonModal {
    NONE=0,
    G4=1 << 0,
    G10=1 << 1,
    G28=1 << 2,
    G30=1 << 3,
    G53=1 << 4,
    G92=1 << 5,
    G92_1=1 << 6,
    G92_2=1 << 7,
    G92_3=1 << 8,
    M114=1 << 9,
    M115=1 << 10,
    M116=1 << 11,
    M117=1 << 12,
    M240=1 << 13,
    M503=1 << 14,
};

typedef struct {
    Position current = {0, 0};
    Position target = {0, 0};
    Position origin = {0, 0};
    float p = 0;
    float q = 0;
    float r = 0;
    float f = 0;
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
    Stream& serial;
    MultiDriver& motors;
    Camera& camera;
    MPU& mpu;
    Battery& battery;
    short int horiz_gear_ratio = 1;
    short int vert_gear_ratio = 1;

    Command cmd;

public:
    GCode(Stream& serial, MultiDriver& motors, Camera& camera, MPU& mpu, Battery& battery)
        :serial(serial), motors(motors), camera(camera), mpu(mpu), battery(battery){};
    void begin(){};
    void setGearRatio(short int horiz, short int vert){
        horiz_gear_ratio = horiz;
        vert_gear_ratio = vert;
    }
    void execute(char buffer[], const char* eob);
    void move(Motion motion, Coords coords, Position& current, Position& target);
};

#endif /* GCODE_H_ */
