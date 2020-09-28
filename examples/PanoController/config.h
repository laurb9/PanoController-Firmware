/*
 * Pano Controller Master Configuration File
 */

#if !defined(REVISION)
#define REVISION __DATE__
#endif

// Battery monitoring settings
#define VCC 3300

// MPU (accel/gyro)
#define MPU_I2C_ADDRESS 0x68
 
#include "platform_gigapan.h"
#include "platform_panocontroller_v4.h"
#include "platform_mecha_e1.h"
#ifndef MOTOR_STEPS
#warning "Missing PLATFORM_ setting, default to PLATFORM_GIGAPAN"
#define PLATFORM_GIGAPAN
#include "platform_gigapan.h"
#endif

// board-specific pin settings
#if defined(ARDUINO_SAMD_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0)
#include "config_feather_m0.h"

#elif defined(ESP32)
#include "config_esp32.h"

#elif defined(__arm__) && defined(CORE_TEENSY)
#include "config_teensy.h"

#elif defined(__AVR__)
#error "AVR is not supported"

#else
#error "Unsupported board"
#endif
