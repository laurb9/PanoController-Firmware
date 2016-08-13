/*
 * Pano Controller Configuration File for Teensy boards from PJRC
 * Supported:
 * - Teensy LC
 * - Teensy 3.1
 * - Teensy 3.2
 */

// Camera shutter controls
#define CAMERA_FOCUS 0
#define CAMERA_SHUTTER 1

// Battery measurement pin R1/R2
#define BATTERY A0

// Joystick inputs
#define JOYSTICK_X A3
#define JOYSTICK_Y A2
#define JOYSTICK_SW 2

// IR remote
#define REMOTE_IN 3

// MPU (accel/gyro)
#define MPU_INT 7

// Future devices
//#define COMPASS_DRDY 4

// Stepper motors and drivers
#define MOTOR_STEPS 200
#define VERT_DIR 5
#define VERT_STEP 6
#define HORIZ_DIR 8
#define HORIZ_STEP 9
#define DRV_M0 10
#define DRV_M1 11

// this should be hooked up to nSLEEP on both drivers
#define MOTORS_ON 13

/*
### Teensy LC pin allocation worksheet

0           CAMERA_FOCUS
1           CAMERA_SHUTTER
2           JOYSTICK_SW
3           REMOTE_IN
4           COMPASS_DRDY
5           VERT_DIR
6           VERT_STEP
7           MPU_INT
8           HORIZ_DIR
9           HORIZ_STEP
10          DRV_M0
MOSI/11     DRV_M1
MISO/12

SCK/13[LED] MOTORS_ON
A0          BATTERY
A1
A2*         JOYSTICK_Y
A3*         JOYSTICK_X
SDA/A4*
SCL/A5*
A6
A7
A8
A9
DAC/A12
 */
