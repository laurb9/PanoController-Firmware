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
#define JOYSTICK_X A2
#define JOYSTICK_Y A3
#define JOYSTICK_SW 20 /* A6 */

// IR remote
#define REMOTE_IN 3

// MPU (accel/gyro)
#define MPU_INT 7

// Future devices
//#define COMPASS_DRDY 4

// Stepper motors and drivers
#define MOTOR_STEPS 200
#define DIR 5
#define VERT_STEP 6
#define HORIZ_STEP 9

// this should be hooked up to nENABLE on both drivers
#define nENABLE 10
