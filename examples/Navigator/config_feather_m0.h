/*
 * Pano Controller Navigator Configuration File for Adafruit Feather M0
 * Supported:
 * - Adafruit Feather M0
 * - Adafruit Feather M0 Bluefruit
 */

// Bluetooth module. These internally connected on the Feather Bluefruit.
#define BLUEFRUIT_SPI_CS    8
#define BLUEFRUIT_SPI_IRQ   7
#define BLUEFRUIT_SPI_RST   4

// Joystick inputs
#define JOYSTICK_X A3
#define JOYSTICK_Y A2
#define JOYSTICK_SW A1

// IR remote is not supported on Feather M0
// https://github.com/z3t0/Arduino-IRremote/issues/274
#define REMOTE_IN -1
