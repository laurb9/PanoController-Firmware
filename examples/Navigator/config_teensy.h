/*
 * Pano Controller Navigator Configuration File for Teensy boards from PJRC
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

// Joystick inputs
#define JOYSTICK_X A3
#define JOYSTICK_Y A2
#define JOYSTICK_SW A1

// IR remote
#define REMOTE_IN 3

// NRF24 radio
#define NRF24_CE A6
#define NRF24_CSN A7
