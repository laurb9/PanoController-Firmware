/*
 * Pano Controller Configuration File for Adafruit Feather M0
 * Supported:
 * - Adafruit Feather M0
 * - Adafruit Feather M0 Bluefruit
 */

// Bluetooth module. These internally connected on the Feather Bluefruit.
#define BLUEFRUIT_SPI_CS    8
#define BLUEFRUIT_SPI_IRQ   7
#define BLUEFRUIT_SPI_RST   4

// Camera shutter controls
#define CAMERA_FOCUS 0
#define CAMERA_SHUTTER 1

// Battery measurement pin R1/R2
#define BATTERY A4

// Joystick inputs
#define JOYSTICK_X A3
#define JOYSTICK_Y A2
#define JOYSTICK_SW A1
#define CANCEL_PIN A0

// IR remote is not supported on Feather M0
// https://github.com/z3t0/Arduino-IRremote/issues/274
#define REMOTE_IN -1

// MPU (accel/gyro)
#define MPU_INT 12

// Future devices
//#define COMPASS_DRDY 11

// Stepper drivers control
#define DIR 5
#define VERT_STEP 6
#define HORIZ_STEP 9

// this should be hooked up to nENABLE on both drivers
#define nENABLE 10
