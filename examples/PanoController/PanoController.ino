/*
 * Pano Controller Firmware
 *
 * Copyright (C)2015-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>
#include <SPI.h>
#include <DRV8834.h>
#include <SyncDriver.h>
#include "config.h"
#include "camera.h"
#include "mpu.h"
#include "battery.h"
#include "gcode.h"
#include "compat.h"

#if defined(BLUEFRUIT_SPI_CS)
#include "ble_bluefruit_spi.h"
static Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
static Bluetooth bluetooth(ble);
#elif defined(ESP32)
#include "ble_esp32.h"
static Bluetooth ble;
#endif

static Battery battery(BATTERY, BATT_R1, BATT_R2, VCC);
static Camera camera(CAMERA_FOCUS, CAMERA_SHUTTER);
static MPU mpu(MPU_I2C_ADDRESS, MPU_INT);
static DRV8834 horiz_motor(MOTOR_STEPS, DIR, HORIZ_STEP, nENABLE);
static DRV8834 vert_motor(MOTOR_STEPS, DIR, VERT_STEP);
static SyncDriver motors(horiz_motor, vert_motor);
static GCode gcode(ble, motors, camera, mpu, battery);

void setup() {
    Serial.begin(115200);
    delay(3000); // wait for serial
    Serial.println("PanoController " GIT_VERSION);
    Serial.println("On " ARDUINO_VARIANT " board " ARDUINO_BOARD);
    Serial.println("Platform " PANO_PLATFORM);

    // turn on power to MPU but give it time to stabilize
    pinMode(MPU_VCC, OUTPUT);
    digitalWrite(MPU_VCC, HIGH);

    Serial.println("Configuring stepper drivers");

    horiz_motor.begin(MOTOR_RPM, MICROSTEPS);
    vert_motor.begin(MOTOR_RPM/2, MICROSTEPS);
    motors.disable(); // turn off motors at startup
    Serial.print("Horiz RPM="); Serial.println(horiz_motor.getRPM());
    Serial.print("Vert  RPM="); Serial.println(vert_motor.getRPM());

#if !defined(ESP32)
    Serial.println("Configuring Bluefruit LE");
    bluetooth.begin();
#else
    Serial.println("Starting Bluetooth");
    ble.begin("PanoController");
#endif

    Serial.print("Checking battery voltage... ");
    battery.begin();
    Serial.print(battery.voltage()); Serial.println("mV");

    Serial.println("Configuring MPU...");
    mpu.begin();

    Serial.println("Initializing G-Code interpreter...");
    gcode.begin();
    gcode.setMaxRPM(MOTOR_RPM, MOTOR_RPM/2);
    gcode.setGearRatio(HORIZ_GEAR_RATIO, VERT_GEAR_RATIO);
    gcode.setMaxAccel(MOTOR_ACCEL, MOTOR_DECEL);

    Serial.println("System ready.");
    while (!ble.isConnected()){
        delay(1000);
    }
}

#define GCODE_BUF_SIZE 128
void loop() {
    static char buffer[GCODE_BUF_SIZE+1];
    static int len;

    if (ble.available()){
        len = ble.readBytesUntil('\n', buffer, GCODE_BUF_SIZE);
        *(buffer+len) = '\0';

        Serial.print("BLE> ");
        Serial.println(buffer);
        gcode.execute(buffer);
        ble.println("ok");
    }

    /*
     * Yield to other processes (ESP8266 and others)
     */
    yield();
}
