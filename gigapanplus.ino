/*
 * GigapanPlus for Arduino project
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DRV8834.h>
#include "pano.h"
#include "camera.h"
#include "joystick.h"

// Address of I2C OLED display. If screen looks scaled edit Adafruit_SSD1306.h
// and pick SSD1306_128_64 or SSD1306_128_32 that matches display type.
#define DISPLAY_I2C_ADDRESS 0x3C
#define OLED_RESET 4

#define JOYSTICK_X A2
#define JOYSTICK_Y A3
#define JOYSTICK_SW 2
#define SDA A4
#define SCL A5

#define MOTOR_STEPS 200
#define VERT_EN 4
#define VERT_DIR 5
#define VERT_STEP 6
#define HORZ_EN 7
#define HORZ_DIR 8
#define HORZ_STEP 9

#define DRV_M0 10
#define DRV_M1 11
#define CAMERA_FOCUS 12
#define CAMERA_SHUTTER 13

static Motor horiz_motor(MOTOR_STEPS, HORZ_DIR, HORZ_STEP, DRV_M0, DRV_M1);
static Motor vert_motor(MOTOR_STEPS, VERT_DIR, VERT_STEP, DRV_M0, DRV_M1);
static Adafruit_SSD1306 display(OLED_RESET);
static Camera camera(CAMERA_FOCUS, CAMERA_SHUTTER);
static Joystick joystick(JOYSTICK_SW, JOYSTICK_X, JOYSTICK_Y);
static Pano pano(horiz_motor, vert_motor, camera);

void setup() {
    Serial.begin(9600);

    delay(100);  // give time for display to init; if display blank increase delay
    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS);
    display.setRotation(2);
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    Serial.print("Ready\n");
    display.display();

    pinMode(VERT_EN, OUTPUT);
    pinMode(HORZ_EN, OUTPUT);
    digitalWrite(VERT_EN, HIGH);
    digitalWrite(HORZ_EN, HIGH);
}

void loop() {
    display.clearDisplay();
    display.setCursor(0,0);
    camera.shutter(250);
    display.print("Start\n");
    display.display();
    digitalWrite(HORZ_EN, LOW);
    horiz_motor.rotate(10);
    digitalWrite(HORZ_EN, HIGH);
    digitalWrite(VERT_EN, LOW);
    vert_motor.rotate(5);
    digitalWrite(VERT_EN, HIGH);
    delay(1000);
}
