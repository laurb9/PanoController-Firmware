/*
 * Pano Controller Configuration File for Adafruit Feather M0
 * Supported:
 * - Adafruit Feather M0
 * - Adafruit Feather M0 Bluefruit
 */

// Address of I2C OLED display. If screen looks scaled edit Adafruit_SSD1306.h
// and pick SSD1306_128_64 or SSD1306_128_32 that matches display type.
#define DISPLAY_I2C_ADDRESS 0x3C
#define OLED_RESET 12
#define TEXT_SIZE 1
#define DISPLAY_ROWS SSD1306_LCDHEIGHT/8/TEXT_SIZE
#define DISPLAY_COLS SSD1306_LCDWIDTH/6/TEXT_SIZE

// Camera shutter controls
#define CAMERA_FOCUS 0
#define CAMERA_SHUTTER 1

// Battery measurement settings
#define VCC 3300
#define LOW_BATTERY 7000
// R1/R2 is the voltage divisor in Ω (GND-R1-A0-R2-Vin)
// measure resistors and enter actual values for a more accurate voltage
#define BATT_R1 9980
#define BATT_R2 46500
#define BATT_RANGE (VCC * (BATT_R1 + BATT_R2) / BATT_R1)
#define BATTERY A0

// Joystick inputs
#define JOYSTICK_X A3
#define JOYSTICK_Y A2
#define JOYSTICK_SW A4

// IR remote
#define REMOTE_IN A5

// MPU (accel/gyro)
#define MPU_I2C_ADDRESS 0x68
#define MPU_INT 10

// Future devices
#define COMPASS_DRDY 11

// Stepper motors and drivers
#define MOTOR_STEPS 200
#define VERT_DIR 5
#define VERT_STEP 6
#define HORIZ_DIR VERT_DIR
#define HORIZ_STEP 9
#define DRV_M0 10
#define DRV_M1 11

// this should be hooked up to nSLEEP on both drivers
#define MOTORS_ON 13
