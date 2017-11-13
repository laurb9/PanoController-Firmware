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
enum M503Options {M503_MOTION=1, M503_BATTERY=2, M503_CAMERA=4};
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
    M202=1 << 13,
    M203=1 << 14,
    M240=1 << 15,
    M503=1 << 16,
};

typedef struct {
    Position current = {0, 0};
    Position target = {0, 0};
    Position origin = {0, 0};
    float p = 0;
    float q = 0;
    float r = 0;
    float s = 0;
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
    Motor& horiz_motor;
    Motor& vert_motor;
    Camera& camera;
    MPU& mpu;
    Battery& battery;
    MultiDriver& motors;
    short horiz_gear_ratio = 1;
    short vert_gear_ratio = 1;
    short max_accel, horiz_accel, vert_accel = 1000;
    short max_decel, horiz_decel, vert_decel = 1000;
    short max_horiz_rpm = 90;
    short max_vert_rpm = 90;
    bool motors_on = false;

    Command cmd;

    GCode(Stream& serial, MultiDriver& motors, Motor& horiz_motor, Motor& vert_motor, Camera& camera, MPU& mpu, Battery& battery)
        :serial(serial), motors(motors), horiz_motor(horiz_motor), vert_motor(vert_motor), camera(camera), mpu(mpu), battery(battery) {
    };
public:
    GCode(Stream& serial, MultiDriver& motors, Camera& camera, MPU& mpu, Battery& battery)
        :GCode(serial, motors, motors.getMotor(0), motors.getMotor(1), camera, mpu, battery) {
    }
    void begin(){
    };
    /*
     * Set motor and platform parameters. These should be set and exposed in the Motor class
     */
    void setMaxAccel(short accel, short decel){
        max_accel = horiz_accel = vert_accel = accel;
        max_decel = horiz_decel = vert_decel = decel;
    }
    void setGearRatio(short horiz, short vert){
        horiz_gear_ratio = horiz;
        vert_gear_ratio = vert;
    }
    void setMaxRPM(short horiz, short vert){
        max_horiz_rpm = horiz;
        max_vert_rpm = vert;
        horiz_motor.setRPM(horiz);
        vert_motor.setRPM(vert);
    }
    void execute(char buffer[]);
    void move(Motion motion, Coords coords, Position& current, Position& target);
    void setAccel(float horiz_accel, float vert_accel);
    void setSpeed(Speed speed, short horiz_rpm, short vert_rpm);
};

#endif /* GCODE_H_ */
