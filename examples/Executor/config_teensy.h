/*
 * Pano Controller Configuration File for Teensy boards from PJRC
 * Supported:
 * - Teensy LC
 * - Teensy 3.1
 * - Teensy 3.2
 */

// Bluetooth module SPI
#define BLUEFRUIT_SPI_CS    8
#define BLUEFRUIT_SPI_IRQ   7
#define BLUEFRUIT_SPI_RST   4   // Optional but recommended, set to -1 if unused

#define SPI_MOSI           11
#define SPI_MISO           12
#define SPI_SCK            13

// NRF24 radio
#define NRF24_CE A6
#define NRF24_CSN A7

// Camera shutter controls
#define CAMERA_FOCUS 0
#define CAMERA_SHUTTER 1

// Battery measurement pin R1/R2
#define BATTERY A0

// MPU (accel/gyro)
#define MPU_INT 2

// Future devices
//#define COMPASS_DRDY 4

// Stepper drivers control
#define DIR 5
#define VERT_STEP 6
#define HORIZ_STEP 9

// this should be hooked up to nENABLE on both drivers
#define nENABLE 10
