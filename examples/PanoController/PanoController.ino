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
#include <MultiDriver.h>
#include "config.h"
#include "camera.h"
#include "display.h"
#include "mpu.h"
#include "battery.h"
#include "bluetooth.h"
#include "gcode.h"

// static Display display(OLED_RESET);

static Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
static Bluetooth bluetooth(ble);

static Battery battery(BATTERY, BATT_R1, BATT_R2, VCC);
static Camera camera(CAMERA_FOCUS, CAMERA_SHUTTER);
static MPU mpu(MPU_I2C_ADDRESS, MPU_INT);
static DRV8834 horiz_motor(MOTOR_STEPS, DIR, HORIZ_STEP, nENABLE);
static DRV8834 vert_motor(MOTOR_STEPS, DIR, VERT_STEP);
static MultiDriver motors(horiz_motor, vert_motor);
static GCode gcode(ble, motors, camera, mpu, battery);

void setup() {
    Serial.begin(115200);
    delay(2000); // wait for serial
    Serial.println("PanoController built " __DATE__ " " __TIME__);

/*
    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS, false);
    display.setRotation(0);
    display.clearDisplay();
    display.setTextCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(TEXT_SIZE);
*/

    Serial.println("Configuring MPU");
    mpu.begin();

    Serial.println("Configuring stepper drivers");
    horiz_motor.begin(MOTOR_RPM, MICROSTEPS);
    vert_motor.begin(MOTOR_RPM, MICROSTEPS);
    motors.disable(); // turn off motors at startup
    horiz_motor.setSpeedProfile(DRV8834::LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
    vert_motor.setSpeedProfile(DRV8834::LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);

    Serial.println("Configuring Bluefruit LE");
    ble.begin(true);
    ble.echo(false);     // disable command echo
    ble.factoryReset();
    ble.info();
    ble.verbose(false);  // turn off debug info

    Serial.println("Initializing BLE App Communication");
    bluetooth.begin();

    ble.sendCommandCheckOK("AT+BLEBATTEN=1"); // enable battery service
    // ble.sendCommandCheckOK("AT+BLEPOWERLEVEL=0"); // can be used to change transmit power

    Serial.print("Checking battery voltage... ");
    battery.begin();
    Serial.print(battery.voltage()); Serial.println("mV");

    Serial.println("Initializing G-Code interpreter...");
    gcode.begin();
    gcode.setGearRatio(HORIZ_GEAR_RATIO, VERT_GEAR_RATIO);

    Serial.println("System ready.");
    
    while (!ble.isConnected()){
        delay(1000);
    }
}

#define GCODE_BUF_SIZE 128
void loop() {
    static char buffer[GCODE_BUF_SIZE+1];
    static char *eob = buffer + GCODE_BUF_SIZE;
    static int len;

    if (ble.available()){
        len = ble.readline(buffer, GCODE_BUF_SIZE);
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
