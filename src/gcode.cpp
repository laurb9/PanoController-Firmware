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

#define d(x) serial.print(x)

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
    cmd.p = 0;
    cmd.lineno = 0;
    cmd.nonmodal = NONE;

    while (buffer < eob){
        char* next = buffer;
        float value;
        char letter = toupper(*buffer++);
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
            case 114: cmd.nonmodal |= NonModal::M114; break; // get current position
            case 115: cmd.nonmodal |= NonModal::M115; break; // get firmware version and capabilities
            case 116: cmd.nonmodal |= NonModal::M116; break; // zero-motion wait
            case 117: cmd.nonmodal |= NonModal::M117; break; // get origin position
            case 240: cmd.nonmodal |= NonModal::M240; break; // shutter
            case 320: cmd.speed = Speed::ACCEL; break;
            case 321: cmd.speed = Speed::CONSTANT; break;
            case 503: cmd.nonmodal |= NonModal::M503; break; // get current settings

            case 17:
                motors.enable();
                // assume new origin since motors could have been moved
                cmd.origin.a = cmd.origin.c = 0;
                break;
            case 18: 
                motors.disable();
                break;
            };
            break;

        /* Variables */
        case 'A': cmd.target.a = value; break;
        case 'C': cmd.target.c = value; break;
        case 'P': cmd.p = value; break;
        case 'Q': cmd.q = value; break;
        case 'R': cmd.r = value; break;
        case 'F': cmd.f = value; break;
        };
    }

    /*
     * Execute the command
     * order of execution table (just as reference, we do not actually need all of it)
     * https://www.tormach.com/order_execution_table.html
     */

    // acceleration ("feed rate")
    if (cmd.speed == Speed::CONSTANT){
        // motors.setSpeedProfile(MultiDriver::CONSTANT_SPEED);
    } else {
        // motors.setSpeedProfile(MultiDriver::LINEAR_SPEED, accel, decel);
    }

    // Camera operations
    if (cmd.nonmodal & NonModal::M116){
        // Zero-Motion wait
        int start = millis();
        bool success = mpu.zeroMotionWait(100 * cmd.q, 1000 * cmd.p);
        d("ZM="); d(millis()-start);
        d((success) ? "\n" : " (failed)\n");
    }
    if (cmd.nonmodal & NonModal::M240){
        // Shutter
        camera.shutter(1000 * cmd.p, bool(cmd.q));
        if (cmd.r > 0){
            delay(1000 * cmd.r);
        }
    }    

    // dwell
    if (cmd.nonmodal & NonModal::G4){
        delay(1000 * cmd.p);
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
        if (cmd.target.a != 0 || cmd.target.c != 0){
            move(cmd.motion, cmd.coords, cmd.current, cmd.target);
        }
    }

    // Data Query. These are here so we can read the final state of a move.
    if (cmd.nonmodal & NonModal::M115){
        // firmware version and capabilities
        d("PanoController 2.2beta build "  __DATE__ " " __TIME__ "\n");
        d("Options CAMERA BATT ZM\n");
        d("Axis A C\n");
    }
    if (cmd.nonmodal & NonModal::M503){
        // current settings
        d("GearRatioA=1:");d(horiz_gear_ratio);d("\n");
        d("GearRatioC=1:");d(vert_gear_ratio);d("\n");
        d("Battery=");d(battery.voltage());d("\n");
        d("ShutterConnected=");d(camera.isShutterConnected() ? "true\n" : "false\n");
        d("FocusConnected=");d(camera.isFocusConnected() ? "true\n" : "false\n");
    }
    if (cmd.nonmodal & NonModal::M117){
        // print origin
        d("OriginA=");d(cmd.origin.a);d("\n");
        d("OriginC=");d(cmd.origin.c);d("\n");
    }
    if (cmd.nonmodal & NonModal::M114){
        // current position
        d("CurrentA=");d(cmd.current.a);d("\n");
        d("CurrentC=");d(cmd.current.c);d("\n");
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
    }
}

void GCode::move(Motion motion, Coords coords, Position& current, Position& target){
    if (coords == Coords::RELATIVE){
        motors.rotate(target.a * horiz_gear_ratio, target.c * vert_gear_ratio);
        current.a += target.a;
        current.c += target.c;                    
    } else {
        motors.rotate((target.a - current.a) * horiz_gear_ratio, 
                      (target.c - current.c) * vert_gear_ratio);
        current.a = target.a;
        current.c = target.c;
    };
}
