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
#define dln(x) serial.println(x)

// convert motor RPM to platform °/s
#define platformSpeed(rpm, gear_ratio) (6.0 * rpm / gear_ratio)
// convert platform °/s to motor RPM
#define motorRPM(speed, gear_ratio) (speed * gear_ratio / 6.0)
// convert motor acceleration in steps/s^2 to platform °/s^2
#define platformAccel(accel, steps, gear_ratio) (360.0 * accel / steps / gear_ratio)
// convert platform acceleration in °/s^2 to motor acceleration steps/s^2
#define motorAccel(plat_accel, steps, gear_ratio) long(plat_accel * gear_ratio * steps / 360.0)

/*
 * Parse and execute a G-code command
 * Example program line
 * G1 G91 A5.3 C-0.5 (rotation by 5.3° horiz and -0.5° vert)
 */
void GCode::execute(char buffer[]){
    /*
     * Parse the command line
     */
    // reset the parameters
    cmd.target.a = 0;
    cmd.target.c = 0;
    cmd.p = 0;
    cmd.lineno++;
    cmd.nonmodal = NONE;

    while (*buffer){
        char* next = buffer;
        float value;
        char letter = toupper(*buffer++);
        if (letter == ' ') continue;
        value = strtod(buffer, &next);

        switch (letter){
        case 'N': continue;
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
            case 30: cmd.wait = Wait::M30; break; // end program
            case 17: motors_on = true; break;
            case 18: motors_on = false; break;
            case 114: cmd.nonmodal |= NonModal::M114; break; // get current position
            case 115: cmd.nonmodal |= NonModal::M115; break; // get firmware version and capabilities
            case 116: cmd.nonmodal |= NonModal::M116; break; // zero-motion wait
            case 117: cmd.nonmodal |= NonModal::M117; break; // get origin position
            case 202: cmd.nonmodal |= NonModal::M202; break; // set acceleration
            case 203: cmd.nonmodal |= NonModal::M203; break; // set (max) speed
            case 240: cmd.nonmodal |= NonModal::M240; break; // shutter
            case 320: cmd.speed = Speed::ACCEL; break;
            case 321: cmd.speed = Speed::CONSTANT; break;
            case 503: cmd.nonmodal |= NonModal::M503; break; // get current settings
            };
            break;

        /* Variables */
        case 'A': cmd.target.a = value; break; // should not write directly to these
        case 'C': cmd.target.c = value; break;
        case 'P': cmd.p = value; break;
        case 'Q': cmd.q = value; break;
        case 'R': cmd.r = value; break;
        case 'S': cmd.s = value; break;
        case 'F': cmd.f = value; break;
        };
    }

    /*
     * Execute the command
     * order of execution table (just as reference, we do not actually need all of it)
     * https://www.tormach.com/order_execution_table.html
     */

    // acceleration and speed ("feed rate")
    if (cmd.nonmodal & NonModal::M202){
        // Acceleration
        setAccel(cmd.target.a, cmd.target.c);
        cmd.target.a = 0;
        cmd.target.c = 0;
    } else if (cmd.nonmodal & NonModal::M203){
        // Speed
        setSpeed(cmd.speed, cmd.target.a, cmd.target.c);
        cmd.target.a = 0;
        cmd.target.c = 0;
    } else {
        setSpeed(cmd.speed, 0, 0);
    }
    
    // Camera operations
    if (cmd.nonmodal & NonModal::M116){
        // Zero-Motion wait
        unsigned long start = millis();
        bool success = mpu.zeroMotionWait(int(cmd.q*100), 1000 * cmd.s);
        d("ZeroMotionWait="); dln(float(millis()-start)/1000);
    }
    if (cmd.nonmodal & NonModal::M240){
        // Shutter
        camera.shutter(1000 * cmd.s, bool(cmd.q));
        if (cmd.r > 0){
            delay(1000 * cmd.r);
        }
    }    

    // dwell
    if (cmd.nonmodal & NonModal::G4){
        delay((cmd.s > 0) ? 1000 * cmd.s : cmd.p);
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

    // power steppers on / off 
    if (motors_on && battery.voltage() > 3000){
        motors.enable();
    } else {
        motors.disable();
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
        // Firmware version, capabilities and static configuration
        // These only need to be read once, at connection time.
        dln("Name=PanoController");
        dln("Version=2.2beta");
        dln("Build="  __DATE__ " " __TIME__);
        dln("BatteryMin=6.0"); // FIXME: should come from motor
        d("MaxSpeedA="); dln(platformSpeed(max_horiz_rpm, horiz_gear_ratio));
        d("MaxSpeedC="); dln(platformSpeed(max_vert_rpm, vert_gear_ratio));
        /*
        // these don't seem useful to the app, now that we convert to/from internal values
        d("GearRatioA=1:"); dln(horiz_gear_ratio);
        d("GearRatioC=1:"); dln(vert_gear_ratio);
        */
        d("MaxAccelA="); dln(platformAccel(max_accel, horiz_motor.getSteps(), horiz_gear_ratio));
        d("MaxAccelC="); dln(platformAccel(max_accel, vert_motor.getSteps(), vert_gear_ratio));
        d("MaxDecelA="); dln(platformAccel(max_decel, horiz_motor.getSteps(), horiz_gear_ratio));
        d("MaxDecelC="); dln(platformAccel(max_decel, vert_motor.getSteps(), vert_gear_ratio));
        d("MovePrecisionA=");serial.println(1.0/horiz_gear_ratio/horiz_motor.calcStepsForRotation(1.0), 4);
        d("MovePrecisionC=");serial.println(1.0/vert_gear_ratio/vert_motor.calcStepsForRotation(1.0), 4);
    }
    if (cmd.nonmodal & NonModal::M503){
        // current dynamic state
        int code = cmd.p;
        if (!code || code & M503Options::M503_MOTION){
            d("MotorsEnabled=");dln(motors_on ? "true" : "false");
            d("MotionMode=");dln(cmd.motion);
            d("CoordsMode=");dln((cmd.coords==Coords::ABSOLUTE) ? "ABSOLUTE" : "RELATIVE");
            d("SpeedMode=");dln((cmd.speed==Speed::CONSTANT) ? "CONSTANT" : "ACCELERATED");
            d("SpeedA=");dln(platformSpeed(horiz_motor.getRPM(), horiz_gear_ratio));
            d("SpeedC=");dln(platformSpeed(vert_motor.getRPM(), vert_gear_ratio));
            d("AccelA=");dln(platformAccel(horiz_accel, horiz_motor.getSteps(), horiz_gear_ratio));
            d("AccelC=");dln(platformAccel(vert_accel, vert_motor.getSteps(), vert_gear_ratio));
            d("DecelA=");dln(platformAccel(horiz_decel, horiz_motor.getSteps(), horiz_gear_ratio));
            d("DecelC=");dln(platformAccel(vert_decel, vert_motor.getSteps(), vert_gear_ratio));
        }
        if (!code || code & M503Options::M503_BATTERY){
            d("Battery=");dln(battery.voltage()/1000.0);
        }
        if (!code || code & M503Options::M503_CAMERA){
            d("ShutterConnected=");dln(camera.isShutterConnected() ? "true" : "false");
            d("FocusConnected=");dln(camera.isFocusConnected() ? "true" : "false");
        }
    }
    if (cmd.nonmodal & NonModal::M117){
        // print origin
        d("OriginA=");dln(cmd.origin.a);
        d("OriginC=");dln(cmd.origin.c);
    }
    if (cmd.nonmodal & NonModal::M114){
        // current position
        d("CurrentA=");dln(cmd.current.a);
        d("CurrentC=");dln(cmd.current.c);
    }
    
    // pause
    switch (cmd.wait){
    case Wait::M0:
        dln("STOP");
        break;
    case Wait::M1:
        if (false){ // FIXME: replace w/ check button
            dln("STOP");
        }
        break;
    case Wait::M2:
    case Wait::M30:
        dln("END");
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
    // Wrap-around horizontal position (360° = 0°)
    // This should be an option because it might confuse an unsuspecting client
    if (current.a > 360.0){
        current.a = current.a - 360 * int(current.a / 360);
    }
}
/*
 * Set acceleration
 */
void GCode::setAccel(float horiz_plat_accel, float vert_plat_accel){
    horiz_accel = min(max_accel, motorAccel(horiz_plat_accel, horiz_motor.getSteps(), horiz_gear_ratio));
    vert_accel = min(max_accel, motorAccel(vert_plat_accel, vert_motor.getSteps(), vert_gear_ratio));
    horiz_decel = horiz_accel;
    vert_decel = vert_accel;
}
/*
 * Set speed profile
 */
void GCode::setSpeed(Speed speed, short horiz_speed, short vert_speed){
    short horiz_rpm = (horiz_speed > 0) ? motorRPM(horiz_speed, horiz_gear_ratio) : horiz_motor.getRPM();
    short vert_rpm = (vert_speed > 0) ? motorRPM(vert_speed, vert_gear_ratio) : vert_motor.getRPM();
    switch (cmd.speed){
    case Speed::ACCEL:
        horiz_motor.setSpeedProfile(Motor::LINEAR_SPEED, horiz_accel, horiz_decel);
        vert_motor.setSpeedProfile(Motor::LINEAR_SPEED, vert_accel, vert_decel);
        horiz_motor.setRPM(min(horiz_rpm, max_horiz_rpm));
        vert_motor.setRPM(min(vert_rpm, max_vert_rpm));
        break;

    case Speed::CONSTANT:
        horiz_motor.setSpeedProfile(Motor::CONSTANT_SPEED);
        vert_motor.setSpeedProfile(Motor::CONSTANT_SPEED);
        horiz_motor.setRPM(min(horiz_rpm, max_horiz_rpm/10));
        vert_motor.setRPM(min(vert_rpm, max_vert_rpm/10));
        break;
    }
}