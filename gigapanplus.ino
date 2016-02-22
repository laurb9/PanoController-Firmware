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
#define DISPLAY_ROWS SSD1306_LCDHEIGHT/8

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

// these variables are modified by the menu
volatile int focal, horiz, vert, shutter, pre_shutter, running, orientation, aspect, shots, motors_enable, display_invert;

void setup() {
    Serial.begin(38400);
    delay(1000); // wait for serial
    delay(100);  // give time for display to init; if display blank increase delay
    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS);
    display.setRotation(0);
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    Serial.println(F("Ready\n"));
}

void displayPanoStatus(void){
    display.clearDisplay();
    display.setCursor(0,0);

    display.print(F("Photo "));
    display.print(pano.position+1);
    display.print(F(" of "));
    display.println(pano.getHorizShots()*pano.getVertShots());
    display.print(F("At "));
    display.print(1+pano.position % pano.getHorizShots());
    display.print(F(" x "));
    display.println(1+pano.position / pano.getHorizShots());

    display.print(F("Focal Length "));
    display.print(focal);
    display.println(F("mm"));
    display.print(F("Lens FOV "));
    display.print(camera.getHorizFOV());
    display.print(F(" x "));
    display.println(camera.getVertFOV());
    display.print(F("Pano FOV "));
    display.print(horiz);
    display.print(F(" x "));
    display.println(vert);
    display.print(F("Grid "));
    display.print(pano.getHorizShots());
    display.print(F(" x "));
    display.println(pano.getVertShots());
    display.println();
    display.display();
}

void handleEvent(int event) {

    if (Joystick::isEventLeft(event)) menu.cancel();
    else if (Joystick::isEventRight(event) || Joystick::isEventClick(event)) menu.select();
    else if (Joystick::isEventDown(event)) menu.next();
    else if (Joystick::isEventUp(event)) menu.prev();

    Serial.println();
    display.clearDisplay();
    display.setCursor(0,0);

    menu.render(display, DISPLAY_ROWS);
    display.display();
}

void positionCamera(void){
    int pos_x, pos_y;
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("Move to top left"));
    display.println(F("  \x1e  \n"
                      "\x11 x \x10\n"
                      "  \x1f"));
    display.display();
    pano.motorsOn();
    while (!Joystick::isEventClick(joystick.read())){
        pos_x = joystick.getPositionX();
        pos_y = joystick.getPositionY();
        if (pos_x){
            horiz_motor.setRPM(80*abs(pos_x)/joystick.range);
            horiz_motor.move(4*pos_x/abs(pos_x));
        }
        if (pos_y){
            vert_motor.setRPM(240*abs(pos_y)/joystick.range);
            vert_motor.move(-4*pos_y/abs(pos_y));
        }
    }
}

/*
 * Display and navigate main menu
 * Only way to exit is by starting the panorama which sets running=true
 */
void displayMenu(void){
    int event;

    menu.open();
    display.clearDisplay();
    display.setCursor(0,0);
    menu.render(display, DISPLAY_ROWS);
    display.display();

    while (!running){
        event = joystick.read();
        if (!event) continue;

        handleEvent(event);
        delay(100);

        display.invertDisplay(display_invert);

        if (motors_enable){
            pano.motorsOn();
        } else {
            pano.motorsOff();
        }
    }
    menu.cancel(); // go back to main menu to avoid re-triggering
}

/*
 * Interrupt handler triggered by button click
 */
volatile static int stop_running = false;
void button_click(){
    stop_running = true;
}
/*
 * Execute panorama from start to finish.
 * Button click interrupts.
 */
void executePano(void){
    pano.setFocalLength(focal);
    pano.setFOV(horiz, vert);
    pano.setShutter(shutter, pre_shutter);
    pano.setShots(shots);

    stop_running = false;
    while (joystick.getButtonState()) delay(20);
    pano.start();
    attachInterrupt(digitalPinToInterrupt(JOYSTICK_SW), button_click, FALLING);

    while (running && !stop_running){
        displayPanoStatus();
        pano.shutter();
        running = pano.next();
    };

    // clean up
    detachInterrupt(digitalPinToInterrupt(JOYSTICK_SW));
    running = false;
    menu.sync();

    pano.end();

    Serial.println((stop_running) ? F("Canceled") : F("Finished"));
    display.println((stop_running) ? F("Canceled") : F("Finished"));
    display.display();
    while (!joystick.read()) delay(20);
}

void loop() {
    displayMenu();

    positionCamera();

    executePano();
}
