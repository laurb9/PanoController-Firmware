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
#include "remote.h"
#include "menu.h"

// inactivity time to turn display off (ms)
#define DISPLAY_SLEEP 5*60*1000

// Address of I2C OLED display. If screen looks scaled edit Adafruit_SSD1306.h
// and pick SSD1306_128_64 or SSD1306_128_32 that matches display type.
#define DISPLAY_I2C_ADDRESS 0x3C
#define OLED_RESET 12
#define TEXT_SIZE 1
#define DISPLAY_ROWS SSD1306_LCDHEIGHT/8/TEXT_SIZE
#define DISPLAY_COLS SSD1306_LCDWIDTH/6/TEXT_SIZE
static Adafruit_SSD1306 display(OLED_RESET);

// Camera shutter controls
#define CAMERA_FOCUS 0
#define CAMERA_SHUTTER 1
static Camera camera(CAMERA_FOCUS, CAMERA_SHUTTER);

// Battery measurement settings
#if defined(__AVR__)
#define VCC 5000
#else
#define VCC 3300
#endif
// R1/R2 is the voltage divisor in Ω (GND-R1-A0-R2-Vin)
// measure resistors and enter actual values for a more accurate voltage
#define BATT_R1 9980
#define BATT_R2 46500
#define BATT_RANGE (VCC * (BATT_R1 + BATT_R2) / BATT_R1)
#define BATTERY A0

// Joystick inputs
#define JOYSTICK_X A3
#define JOYSTICK_Y A2
#define JOYSTICK_SW 2
static Joystick joystick(JOYSTICK_SW, JOYSTICK_X, JOYSTICK_Y);

// IR remote
#define REMOTE_IN 3
static Remote remote(REMOTE_IN);

// Future devices
#define COMPASS_DRDY 4
#define MPU_INT 7

// Stepper motors and drivers
#define HORIZ_RPM 30
#define VERT_RPM HORIZ_RPM*3
#define MOTOR_STEPS 200
#define VERT_DIR 5
#define VERT_STEP 6
#define HORIZ_DIR 8
#define HORIZ_STEP 9
#define DRV_M0 10
#define DRV_M1 11
static DRV8834 horiz_motor(MOTOR_STEPS, HORIZ_DIR, HORIZ_STEP, DRV_M0, DRV_M1);
static DRV8834 vert_motor(MOTOR_STEPS, VERT_DIR, VERT_STEP, DRV_M0, DRV_M1);

// this should be hooked up to nSLEEP on both drivers
#define MOTORS_ON 13
static Pano pano(horiz_motor, vert_motor, camera, MOTORS_ON);

// these variables are modified by the menu
volatile int focal, shutter, pre_shutter, orientation, aspect, shots, motors_enable, display_invert;
volatile int horiz, vert;
volatile int running;

void setup() {
    Serial.begin(38400);

    pinMode(COMPASS_DRDY, INPUT_PULLUP);
    pinMode(MPU_INT, INPUT_PULLUP);

    pinMode(BATTERY, INPUT);
    analogReadRes(10);
    analogReadAveraging(32);

    horiz_motor.setMicrostep(32);
    vert_motor.setMicrostep(32);
    delay(1000); // wait for serial
    delay(100);  // give time for display to init; if display blank increase delay
    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS);
    display.setRotation(2);
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(TEXT_SIZE);
    Serial.println(F("Ready\n"));
}

int readBattery(void){
    return map(analogRead(BATTERY), 0, (1<<10)-1, 0, BATT_RANGE);
}

/*
 * Print battery voltage at cursor, format is #.#V (4 chars)
 */
void displayBatteryStatus(void){
    int battmV = readBattery();
    display.setCursor((17-battmV/10000)*6, 0);
    display.print(battmV/1000);
    display.print('.');
    display.print((battmV % 1000)/100);
    display.print('V');
}

/*
 * Display current panorama status (photo index, etc)
 */
void displayPanoStatus(void){
    display.clearDisplay();
    display.setCursor(0,0);

    display.print(F("Photo "));
    display.print(pano.position+1);
    display.print(F(" of "));
    display.println(pano.getHorizShots()*pano.getVertShots());
    display.print(F("At "));
    display.print(1+pano.getCurRow());
    display.print(F(" x "));
    display.println(1+pano.getCurCol());
    displayPanoSize();
    display.print(F("Battery "));
    displayBatteryStatus();
    displayProgress();
    display.display();
}
/*
 * Display progress information (minutes / progress bar)
 */
void displayProgress(void){
    int photos = pano.getHorizShots() * pano.getVertShots();
    display.setCursor(0, 6*8);
    display.print(pano.getTimeLeft()/60);
    display.println(" minutes");
    for (int i=pano.position * DISPLAY_COLS / photos; i > 0; i--){
        display.print('\xda');
    }
    display.println();
}
/*
 * Display panorama information
 */
void displayPanoInfo(void){
    display.clearDisplay();
    display.setCursor(0,0);
    display.print(F("Focal Length "));
    display.print(focal);
    display.println(F("mm"));
    display.print(F("Lens FOV "));
    display.print(camera.getHorizFOV());
    display.print(F("x"));
    display.println(camera.getVertFOV());
    display.print(F("Pano FOV "));
    display.print(pano.horiz_fov);
    display.print(F("x"));
    display.println(pano.vert_fov);
    display.print(pano.getHorizShots()*pano.getVertShots());
    display.println(F(" photos"));
    displayPanoSize();
    displayProgress();
    displayBatteryStatus();
    display.display();
}

/*
 * Display the panorama grid size
 */
void displayPanoSize(){
    display.print(F("Grid "));
    display.print(pano.getVertShots());
    display.print(F(" x "));
    display.print(pano.getHorizShots());
    display.println();
}

/*
 * Display the four navigation arrows, cancel and OK buttons.
 */
void displayArrows(){
    display.setCursor(0,3*8);
    display.println(F("          \x1e         "));
    display.println(F(" [X]     \x11+\x10    [OK]"));
    display.println(F("          \x1f         "));
    display.display();
}

/*
 * Let user move the camera while optionally recording position
 * @param msg:   title string
 * @param horiz: pointer to store horizontal movement
 * @param vert:  pointer to store vertical movement
 */
bool positionCamera(const char *msg, volatile int *horiz, volatile int *vert){
    int pos_x, pos_y;
    int h = 0, v = 0;

    display.clearDisplay();
    display.setCursor(0,0);
    display.println(msg);
    displayArrows();
    pano.motorsEnable(true);
    unsigned event;
    while (true){
        event = joystick.read() | remote.read();
        if (HID::isEventOk(event) || HID::isEventCancel(event)) break;

        pos_x = joystick.getPositionX();
        if (pos_x == 0){
            if (HID::isEventRight(event)) pos_x = pano.horiz_gear_ratio;
            if (HID::isEventLeft(event)) pos_x = -pano.horiz_gear_ratio;
            horiz_motor.setRPM(HORIZ_RPM/3);
        } else {
            horiz_motor.setRPM(HORIZ_RPM*abs(pos_x)/joystick.range);
            pos_x = pos_x/abs(pos_x);
        }

        pos_y = joystick.getPositionY();
        if (pos_y == 0){
            if (HID::isEventUp(event)) pos_y = pano.vert_gear_ratio;
            if (HID::isEventDown(event)) pos_y = -pano.vert_gear_ratio;
            vert_motor.setRPM(VERT_RPM/3);
        } else {
            vert_motor.setRPM(VERT_RPM*abs(pos_y)/joystick.range);
            pos_y = pos_y/abs(pos_y);
        }

        if (pos_x && (!horiz || h + pos_x > 0)){
            horiz_motor.rotate(pos_x);
            h += pos_x;
        }

        if (pos_y && (!vert || v - pos_y > 0)){
            v += -pos_y;
            vert_motor.rotate(pos_y);
        }

        if (vert && !pos_x && !pos_y){
            pano.setFOV(h / pano.horiz_gear_ratio + camera.getHorizFOV(),
                        v / pano.vert_gear_ratio + camera.getVertFOV());
            pano.computeGrid();
            display.setCursor(0,8*6);
            display.print(F("          "));
            display.setCursor(0,8*6);
            displayPanoSize();
            display.print(pano.horiz_fov);
            display.print("x");
            display.print(pano.vert_fov);
            display.print(" ");
            displayBatteryStatus();
            display.display();
        }
    }
    if (horiz){
        *horiz = h / pano.horiz_gear_ratio + camera.getHorizFOV();
        horiz_motor.setRPM(HORIZ_RPM);
        horiz_motor.rotate(-h);
    }
    if (vert){
        *vert = v / pano.vert_gear_ratio + camera.getVertFOV();
        vert_motor.setRPM(VERT_RPM);
        vert_motor.rotate(v);
    }
    return (HID::isEventOk(event));
}

/*
 * Display and navigate main menu
 * Only way to exit is by starting the panorama which sets running=true
 */
void displayMenu(void){
    int event;
    int last_event = millis();

    menu.open();
    display.clearDisplay();
    display.setCursor(0,0);
    menu.render(display, DISPLAY_ROWS);
    display.display();

    while (!running){
        event = joystick.read() | remote.read();
        if (!event){
            displayBatteryStatus();
            display.display();
            if (millis() - last_event > DISPLAY_SLEEP){
                display.clearDisplay();
                display.display();
                last_event = millis();
            }
            continue;
        }

        last_event = millis();
        if (HID::isEventLeft(event) || HID::isEventCancel(event)) menu.cancel();
        else if (HID::isEventRight(event) || HID::isEventOk(event)) menu.select();
        else if (HID::isEventDown(event)) menu.next();
        else if (HID::isEventUp(event)) menu.prev();

        Serial.println();
        display.clearDisplay();
        display.setCursor(0,0);
        menu.render(display, DISPLAY_ROWS);
        displayBatteryStatus();
        display.display();
        delay(100);

        display.invertDisplay(display_invert);

        pano.motorsEnable(motors_enable);
    }
    menu.cancel(); // go back to main menu to avoid re-triggering
}

/*
 * Interrupt handler triggered by button click
 */
volatile static int button_clicked = false;
void button_click(){
    button_clicked = true;
}
/*
 * Execute panorama from start to finish.
 * Button click interrupts.
 */
void executePano(void){


    button_clicked = false;
    joystick.clear(4000), remote.clear(4000);
    pano.start();
    attachInterrupt(digitalPinToInterrupt(JOYSTICK_SW), button_click, FALLING);

    while (running){
        displayPanoStatus();
        if (!pano.position){
            delay(2000);
        }
        pano.shutter();
        running = pano.next();

        if (button_clicked || remote.read()){
            joystick.clear(1000), remote.clear(1000);
            // button was clicked mid-pano, go in manual mode
            int event;
            displayPanoStatus();
            displayArrows();
            while (running){
                event=joystick.read() | remote.read();
                if (!event) continue;
                if (HID::isEventLeft(event)) pano.prev();
                else if (HID::isEventRight(event)) pano.next();
                else if (HID::isEventUp(event)) pano.moveTo(pano.getCurRow() - 1, pano.getCurCol());
                else if (HID::isEventDown(event)) pano.moveTo(pano.getCurRow() + 1, pano.getCurCol());
                else if (HID::isEventOk(event)) break;
                else if (HID::isEventCancel(event)){
                    running = false;
                    break;
                }
                displayPanoStatus();
                displayArrows();
            }
            button_clicked = false;
        }
    };

    // clean up
    detachInterrupt(digitalPinToInterrupt(JOYSTICK_SW));
    running = false;

    pano.end();

    int wait = 8000;
    while (wait && !joystick.read() && !remote.read()){
        delay(20);
        wait -= 20;
    }
    joystick.clear(4000);
    remote.clear(4000);
}

/*
 * Update common camera and pano settings from external vars
 */
void setPanoParams(void){
    menu.cancel();
    camera.setAspect(aspect);
    camera.setFocalLength(focal);
    pano.setShutter(shutter, pre_shutter);
    pano.setShots(shots);
}

/*
 * Menu callback invoked by selecting "Start"
 */
int onStart(int __){
    setPanoParams();

    // set panorama FOV
    if (!positionCamera("Set Top Left", NULL, NULL) ||
        !positionCamera("Set Bottom Right", &horiz, &vert)){
        return false;
    }

    pano.setFOV(horiz, vert);
    if (!positionCamera("Adjust start pos\nSet exposure & focus", NULL, NULL)){
        return false;
    }
    running = true;
    menu.sync();
    executePano();
    return __;
}

/*
 * Menu callback invoked by selecting "Repeat"
 */
int onRepeat(int __){
    setPanoParams();

    pano.setFOV(horiz, vert);
    if (!positionCamera("Adjust start pos\nSet exposure & focus", NULL, NULL)){
        return false;
    }
    running = true;
    menu.sync();
    executePano();
    return __;
}

/*
 * Menu callback invoked by selecting "Repeat"
 */
int on360(int __){
    setPanoParams();

    // set panorama FOV
    horiz = 360;
    if (!positionCamera("Set Top", NULL, NULL) ||
        !positionCamera("Set Bottom", NULL, &vert)){
        return false;
    }

    pano.setFOV(horiz, vert);
    if (!positionCamera("Adjust start pos\nSet exposure & focus", NULL, NULL)){
        return false;
    }
    running = true;
    menu.sync();
    executePano();
    return __;
}

/*
 * Menu callback for displaying last pano info
 */
int onPanoInfo(int __){
    setPanoParams();
    pano.setFOV(horiz, vert);
    pano.computeGrid();
    displayPanoInfo();
    while (!(joystick.read() || remote.read()));
    joystick.clear(4000);
    remote.clear(4000);
    return __;
}

void loop() {
    displayMenu();
    menu.sync();
}
