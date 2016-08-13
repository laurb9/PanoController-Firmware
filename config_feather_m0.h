/*
 * Pano Controller Configuration File for Adafruit Feather M0
 * Supported:
 * - Adafruit Feather M0
 * - Adafruit Feather M0 Bluefruit
 */

// Camera shutter controls
#define CAMERA_FOCUS 0
#define CAMERA_SHUTTER 1

// Battery measurement pin R1/R2
#define BATTERY A1

// Joystick inputs
#define JOYSTICK_X A2
#define JOYSTICK_Y A3
#define JOYSTICK_SW A4

// IR remote is not supported on Feather M0
// https://github.com/z3t0/Arduino-IRremote/issues/274
#define REMOTE_IN -1

// MPU (accel/gyro)
#define MPU_INT 12

// Future devices
//#define COMPASS_DRDY 11

// Stepper motors and drivers
#define MOTOR_STEPS 200
#define DIR 5
#define VERT_STEP 6
#define HORIZ_STEP 9

// this should be hooked up to nSLEEP on both drivers
#define MOTORS_ON 13

