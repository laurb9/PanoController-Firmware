/*
 * G-Code command interpreter
 *
 * This does not attempt to be a complete or even correct G-Code interpreter. 
 * It implements G-Code only as needed to support the pano functionality, without
 * ruling out future additions.
 *
 * Codes reference: https://www.tormach.com/machine_codes_gcodes.html
 *
 * Copyright (C)2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "gcode.h"

#define d(x) Serial.print(x)

/*
 * Parse and execute a G-code command
 * Example program line
 * G1 G91 A5.3 C-0.5 (rotation by 5.3° horiz and -0.5° vert)
 */
void GCode::execute(char buffer[], const char* eob){
    /*
     * Parse the command line
     */
    // reset the parameters
    cmd.target.a = 0;
    cmd.target.c = 0;
    cmd.delay_ms = 0;
    cmd.lineno = 0;
    cmd.nonmodal = NONE;

    while (buffer < eob){
        char* next = buffer;
        float value;
        char letter = *buffer++;
        if (letter == ' ') continue;
        value = strtod(buffer, &next);

        switch (letter){
        case 'N': cmd.lineno = int(value); continue;
        case ';':
        case '%': break;
        case 'O': break; // program name

        case 'G':
            switch (int(value)){
            case 0: cmd.motion = Motion::G0; break; // rapid move
            case 1: cmd.motion = Motion::G1; break; // linear interpolation move
            case 4: cmd.nonmodal |= NonModal::G4; break; // dwell
            case 28: cmd.nonmodal |= NonModal::G28; break; // go to origin
            case 80: cmd.motion = NO_MOTION; break; // cancel modal mode
            case 90: cmd.coords = Coords::ABSOLUTE; break;
            case 91: cmd.coords = Coords::RELATIVE; break;
            case 92: cmd.nonmodal |= (value == 92.1) ? NonModal::G92_1 : NonModal::G92; break;
            };
            break;

        case 'M':
            switch (int(value)){
            case 0: cmd.wait = Wait::M0; break; // pause, wait for user
            case 1: cmd.wait = Wait::M1; break; // stop if button pressed
            case 2: cmd.wait = Wait::M2; break; // end program
            case 17: motors.enable(); break;
            case 18: motors.disable(); break;
            case 116: cmd.wait = Wait::M116; break; // zero-motion wait
            case 240: cmd.wait = Wait::M240; break; // shutter
            case 320: cmd.speed = Speed::ACCEL; break;
            case 321: cmd.speed = Speed::CONSTANT; break;

            case 117: cmd.nonmodal |= NonModal::M117; break; // get origin position
            case 114: cmd.nonmodal |= NonModal::M114; break; // get current position
            case 115: cmd.nonmodal |= NonModal::M115; break; // get firmware version and capabilities
            case 503: cmd.nonmodal |= NonModal::M503; break; // get current settings
            };
            break;

        /* Variables */
        case 'A': cmd.target.a = value; break;
        case 'C': cmd.target.c = value; break;
        case 'P': cmd.delay_ms = 1000 * value; break;
        case 'S': cmd.delay_ms = value; break;
        case 'F': break; // accel value
        };
    }

    /*
     * Execute the command
     * order of execution table (just as reference, we do not actually need all of it)
     * https://www.tormach.com/order_execution_table.html
     */

    // messages
    if (cmd.nonmodal & NonModal::M117){
        // print origin
        Serial.print("ok origin A");Serial.print(cmd.origin.a);
        Serial.print(" C");Serial.print(cmd.origin.c);
        Serial.println();
    }
    if (cmd.nonmodal & NonModal::M114){
        // current position
        Serial.print("ok current A");Serial.print(cmd.current.a);
        Serial.print(" C");Serial.print(cmd.current.c);
        Serial.println();
    }
    if (cmd.nonmodal & NonModal::M115){
        // firmware version and capabilities
        Serial.println("ok PanoController 1.0");
        Serial.println("axis A360 C175");
        Serial.println("origin A0 C15");
    }
    if (cmd.nonmodal & NonModal::M503){
        // current settings
        Serial.println("ok");
    }

    // acceleration ("feed rate")
    if (cmd.speed == Speed::CONSTANT){
        // motors.setSpeedProfile(MultiDriver::CONSTANT_SPEED);
    } else {
        // motors.setSpeedProfile(MultiDriver::LINEAR_SPEED, accel, decel);
    }

    // dwell
    if (cmd.nonmodal & NonModal::G4){
        delay(cmd.delay_ms);
    }

    // units

    // distance mode

    // set offsets (origin)
    if (cmd.nonmodal & NonModal::G92_1){
        // reset origin
        cmd.origin.a = cmd.target.a = cmd.current.a = 0;
        cmd.origin.c = cmd.target.c = cmd.current.c = 0;
    }
    if (cmd.nonmodal & NonModal::G92){
        // origin moves so current coordinates match target 
        cmd.origin.a -= cmd.target.a + cmd.current.a;
        cmd.origin.c -= cmd.target.c + cmd.current.c;
        cmd.current.a = cmd.target.a;
        cmd.current.c = cmd.target.c;
        cmd.target.a = 0;
        cmd.target.c = 0;
    }

    // motion

    if (cmd.nonmodal & NonModal::G28){
        // move to origin via another point (incremental)
        if (cmd.target.a != 0 || cmd.target.c != 0){
            move(cmd.motion, Coords::RELATIVE, cmd.current, cmd.target);
        }
        cmd.target.a = cmd.origin.a;
        cmd.target.c = cmd.origin.c;
        move(cmd.motion, Coords::ABSOLUTE, cmd.current, cmd.target);
        cmd.target.a = cmd.target.c = 0;
    }
     
    switch (cmd.motion){
    case Motion::G0:
    case Motion::G1:
        move(cmd.motion, cmd.coords, cmd.current, cmd.target);
    }

    // pause
    switch (cmd.wait){
    case Wait::M0:
        d("STOP\n");
        break;
    case Wait::M1:
        if (false){ // FIXME: replace w/ check button
            d("STOP\n");
        }
        break;
    case Wait::M2:
        d("END\n");
        break;
    case Wait::M240:
        camera.shutter(cmd.delay_ms, false);
        break;
    }
}

void GCode::move(Motion motion, Coords coords, Position& current, Position& target){
    d("MOVE ");d(target.a);d(" ");d(target.c);d("\n");
    if (coords == Coords::RELATIVE){
        motors.rotate(target.a, target.c);
        current.a += target.a;
        current.c += target.c;                    
    } else {
        motors.rotate(target.a - current.a, target.c - current.c);
        current.a = target.a;
        current.c = target.c;
    };
    d("AT ");d(cmd.current.a);d(" ");d(cmd.current.c);d("\n");
}
