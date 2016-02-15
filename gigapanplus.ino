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
#include "menu.h"

// Address of I2C OLED display. If screen looks scaled edit Adafruit_SSD1306.h
// and pick SSD1306_128_64 or SSD1306_128_32 that matches display type.
#define DISPLAY_I2C_ADDRESS 0x3C
#define OLED_RESET 4

#define JOYSTICK_X A2
#define JOYSTICK_Y A3
#define JOYSTICK_SW 2

#define MOTOR_STEPS 200
#define VERT_EN 4
#define VERT_DIR 5
#define VERT_STEP 6
#define HORIZ_EN 7
#define HORIZ_DIR 8
#define HORIZ_STEP 9

#define DRV_M0 10
#define DRV_M1 11
#define CAMERA_FOCUS 12
#define CAMERA_SHUTTER 13

static DRV8834 horiz_motor(MOTOR_STEPS, HORIZ_DIR, HORIZ_STEP, DRV_M0, DRV_M1);
static DRV8834 vert_motor(MOTOR_STEPS, VERT_DIR, VERT_STEP, DRV_M0, DRV_M1);
static Adafruit_SSD1306 display(OLED_RESET);
static Camera camera(CAMERA_FOCUS, CAMERA_SHUTTER);
static Joystick joystick(JOYSTICK_SW, JOYSTICK_X, JOYSTICK_Y);
static Pano pano(horiz_motor, vert_motor, camera, HORIZ_EN, VERT_EN);

void setup() {
    Serial.begin(9600);
    delay(1000); // wait for serial
    delay(100);  // give time for display to init; if display blank increase delay
    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS);
    display.setRotation(0);
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    Serial.print(F("Ready\n"));
    display.display();
    displayMenu(EVENT_NONE);
}

void displayMenu(int event) {
    const int rows = 6;
    static int selected = 0;
    static int pointer = 0;
    static int start = 0;
    static union MenuItem current_option;

    Serial.println();
    display.clearDisplay();
    display.setCursor(0,0);

    if (!current_option.option && (isEventRight(event) || isEventClick(event))){
        current_option = menu[selected];
        pointer = current_option.names->pos;
    } else if (current_option.option && isEventLeft(event)){
        current_option.option = NULL;
    } else if (current_option.option && (isEventRight(event) || isEventClick(event))){
        // select
        current_option.names->pos = pointer;
        current_option.option = NULL;
    }

    if (!current_option.option){
        display.println(F("----- Main Menu -----"));
        Serial.println(F("----- Main Menu -----"));

        if (isEventDown(event) && selected < menu_size-1){
            selected++;
            if (selected > rows/2 && start < menu_size-rows){
                start++;
            }
        };
        if (isEventUp(event) && selected > 0){
            selected--;
            if (start > 0){
                start--;
            }
        };
        for (int i=start; i<start+rows && i<menu_size; i++){
            if (i == selected){
                display.setTextColor(BLACK, WHITE);
                Serial.print(F(">"));
            }
            display.println(menu[i].option->description);
            Serial.println(menu[i].option->description);
            if (i == selected){
                display.setTextColor(WHITE, BLACK);
            }
        }
    } else {
        if (isEventDown(event) && pointer < current_option.names->count-1){
            pointer++;
        }
        if (isEventUp(event) && pointer > 0){
            pointer--;
        }
        current_option.names->render(&display, rows, pointer);
    }
    display.display();
}

void loop() {
    int event = joystick.read();
    if (event){
        displayMenu(event);
    }
    /*
    display.clearDisplay();
    display.setCursor(0,0);
    display.print(F("Start "));
    display.display();
    pano.setFocalLength(50);
    pano.setFOV(180, 90);
    pano.setShutter(500, 250);
    pano.start();
    do {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("running\n"));
        display.display();
        display.print(F("pos = "));
        display.println(pano.position);
        display.print(F("horiz = "));
        display.println(pano.horiz_position);
        display.print(F("vert = "));
        display.println(pano.vert_position);
        display.display();
    } while(pano.next());
    pano.end();
    display.println(F("end"));
    display.display();
    delay(10000);
    */
}
