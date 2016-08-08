/*
 * Pano Controller for Arduino project
 *
 * Copyright (C)2015,2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>
#include <DRV8834.h>
#include "pano.h"
#include "camera.h"
#include "hid.h"
#include "joystick.h"
#include "remote.h"
#include "menu.h"
#include "display.h"
#include "mpu.h"

// Address of I2C OLED display. If screen looks scaled edit Adafruit_SSD1306.h
// and pick SSD1306_128_64 or SSD1306_128_32 that matches display type.
#define DISPLAY_I2C_ADDRESS 0x3C
#define OLED_RESET 12
#define TEXT_SIZE 1
#define DISPLAY_ROWS SSD1306_LCDHEIGHT/8/TEXT_SIZE
#define DISPLAY_COLS SSD1306_LCDWIDTH/6/TEXT_SIZE
static Display display(OLED_RESET);

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
#define LOW_BATTERY 7000
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

// HID (Human Interface Device) Combined joystick+remote
static AllHID hid(2, new HID* const[2] {&joystick, &remote});

// MPU (accel/gyro)
#define MPU_I2C_ADDRESS 0x68
#define MPU_INT 7
static MPU mpu(MPU_I2C_ADDRESS, MPU_INT);

// Future devices
#define COMPASS_DRDY 4

// Stepper motors and drivers
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
static Pano pano(horiz_motor, vert_motor, camera, mpu, MOTORS_ON);

// these variables are modified by the menu
volatile int focal, shutter, pre_shutter, post_wait, long_pulse,
             orientation, aspect, shots, motors_enable, display_invert,
             horiz, vert, running;

void setup() {
    Serial.begin(38400);

    pinMode(COMPASS_DRDY, INPUT_PULLUP);

    mpu.init();

    pinMode(BATTERY, INPUT);
#if defined(__MK20DX256__) || defined(__MKL26Z64__)
    analogReadRes(10);
    analogReadAveraging(32);
#endif

    horiz_motor.setMicrostep(32);
    vert_motor.setMicrostep(32);
    delay(1000); // wait for serial
    delay(100);  // give time for display to init; if display blank increase delay
    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS, false);
    display.setRotation(2);
    display.clearDisplay();
    display.setTextCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(TEXT_SIZE);
    Serial.println("Ready\n");
}

int readBattery(void){
    return map(analogRead(BATTERY), 0, (1<<10)-1, 0, BATT_RANGE);
}

/*
 * Print battery voltage at cursor, format is #.#V (4 chars)
 */
void displayBatteryStatus(void){
    int battmV = readBattery();
    display.setTextCursor(0, (17-battmV/10000));
    // poor attempt at blinking
    if (battmV < LOW_BATTERY && millis() & 1024){
        display.setTextColor(BLACK, WHITE);
    }
    display.printf("%d.%dV", battmV/1000, (battmV % 1000)/100);
    display.setTextColor(WHITE, BLACK);
}

/*
 * Display current panorama status (photo index, etc)
 */
void displayPanoStatus(void){
    display.clearDisplay();
    display.setTextCursor(0,0);

    display.printf("Photo %d of %d\n", pano.position+1, pano.getHorizShots()*pano.getVertShots());
    display.printf("At %d x %d\n", 1+pano.getCurRow(), 1+pano.getCurCol());
    displayPanoSize();
    displayBatteryStatus();
    displayProgress();
    display.display();
}
/*
 * Display progress information (minutes / progress bar)
 */
void displayProgress(void){
    int photos = pano.getHorizShots() * pano.getVertShots();
    display.setTextCursor(6, 0);
    display.printf("%d minutes ", pano.getTimeLeft()/60);
    if (pano.steady_delay_avg > 500){
        display.setTextCursor(6, 16);
        display.printf("%2ds ", (pano.steady_delay_avg+500)/1000);
        display.printf((pano.steady_delay_avg < 8000) ? "\x12" : "!");
    }
    display.setTextCursor(7, 0);
    for (int i=(pano.position+1) * DISPLAY_COLS / photos; i > 0; i--){
        display.print('\xda');
    }
    display.println();
}
/*
 * Display panorama information
 */
void displayPanoInfo(void){
    float horiz_fov = camera.getHorizFOV();
    float vert_fov = camera.getVertFOV();
    display.clearDisplay();
    display.setTextCursor(0,0);
    display.printf("Lens: %dmm\n", focal);
    display.printf("      %d.%d x %d.%d\n",
                   int(horiz_fov), round(10*(horiz_fov-int(horiz_fov))),
                   int(vert_fov), round(10*(vert_fov-int(vert_fov))));
    display.printf("Pano FOV %d x %d \n", pano.horiz_fov, pano.vert_fov);
    displayPanoSize();
    display.printf("%d photos\n", pano.getHorizShots()*pano.getVertShots());
    displayProgress();
    displayBatteryStatus();
    display.display();
}

/*
 * Display the panorama grid size
 */
void displayPanoSize(){
    display.printf("Grid %d x %d \n", pano.getVertShots(), pano.getHorizShots());
}

/*
 * Display the four navigation arrows, cancel and OK buttons.
 */
void displayArrows(){
    display.setTextCursor(3, 0);
    display.println("          \x1e         \n"
                    " [X]     \x11+\x10    [OK]\n"
                    "          \x1f         ");
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
    unsigned event;
    int horiz_rpm, vert_rpm;

    display.clearDisplay();
    display.setTextCursor(0,0);
    display.println(msg);
    displayArrows();
    pano.motorsEnable(true);

    if (horiz || vert){
        pano.setMotorsHomePosition();
    }

    while (true){
        event = hid.read();
        if (HID::isEventOk(event) || HID::isEventCancel(event)) break;

        pos_x = joystick.getPositionX();
        horiz_rpm = DYNAMIC_HORIZ_RPM(camera.getHorizFOV());
        if (pos_x == 0){
            if (HID::isEventRight(event)) pos_x = 1;
            if (HID::isEventLeft(event)) pos_x = -1;
            horiz_rpm /= 3;
        } else {
            // proportional speed control
            horiz_rpm = horiz_rpm * abs(pos_x) / joystick.range;
            pos_x = pos_x / abs(pos_x);
        }
        if (pos_x && horiz){
            if (pos_x < -pano.horiz_home_offset){
                pos_x = -pano.horiz_home_offset;
            } else if (abs(pos_x + pano.horiz_home_offset) + camera.getHorizFOV() > 360){
                pos_x = 360 - camera.getHorizFOV() - pano.horiz_home_offset;
            }
        }

        pos_y = joystick.getPositionY();
        vert_rpm = DYNAMIC_VERT_RPM(camera.getVertFOV());
        if (pos_y == 0){
            if (HID::isEventUp(event)) pos_y = 1;
            if (HID::isEventDown(event)) pos_y = -1;
            vert_rpm /= 3;
        } else {
            // proportional speed control
            vert_rpm = vert_rpm * abs(pos_y) / joystick.range;
            pos_y = pos_y / abs(pos_y);
        }
        if (pos_y && vert){
            if (pos_y > -pano.vert_home_offset){
                pos_y = -pano.vert_home_offset;
            } else if (-(pos_y + pano.vert_home_offset) + camera.getVertFOV() > 180){
                pos_y = -(180 - camera.getVertFOV() + pano.vert_home_offset);
            }
        }

        if (vert && !pos_x && !pos_y){
            pano.setFOV((horiz) ? abs(pano.horiz_home_offset) + camera.getHorizFOV() : 360,
                        abs(pano.vert_home_offset) + camera.getVertFOV());
            pano.computeGrid();
            display.setTextCursor(6, 0);
            display.print("          ");
            display.setTextCursor(6, 0);
            displayPanoSize();
            display.printf("FOV %d x %d ", pano.horiz_fov, pano.vert_fov);
            displayBatteryStatus();
            display.display();
        }

        if (pos_x || pos_y){
            horiz_motor.setRPM(horiz_rpm);
            vert_motor.setRPM(vert_rpm);
            pano.moveMotors(pos_x, pos_y);
        }
    }

    if (horiz || vert){
        if (horiz){
            *horiz = abs(pano.horiz_home_offset) + camera.getHorizFOV();
        }
        if (vert){
            *vert = abs(pano.vert_home_offset) + camera.getVertFOV();
        }
        pano.moveMotorsHome();
    }
    return (HID::isEventOk(event));
}

/*
 * Execute panorama from start to finish.
 * Button click interrupts.
 */
void executePano(void){

    hid.clear(4000);
    pano.start();

    while (running){
        displayPanoStatus();
        if (!pano.position){
            delay(2000);
        }
        if (shutter > 0){
            pano.shutter();
        }

        if (shutter == 0 || hid.read()){
            hid.clear(1000);
            // button was clicked mid-pano or we are in manual shutter mode
            int event;
            displayPanoStatus();
            displayArrows();
            while (running){
                event=hid.read();
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
        }
        running = running && pano.next();
    };

    running = false;
    displayPanoStatus();

    pano.end();

    int wait = 8000;
    while (wait && !hid.read()){
        delay(20);
        wait -= 20;
    }
    hid.clear(4000);
}

/*
 * Update common camera and pano settings from external vars
 */
void setPanoParams(void){
    menu.cancel();
    camera.setAspect(aspect);
    camera.setFocalLength(focal);
    pano.setShutter(shutter, pre_shutter, post_wait, long_pulse);
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
    while (!hid.read());
    hid.clear(4000);
    return __;
}

/*
 * Menu callback for testing camera shutter
 */
int onTestShutter(int __){
    setPanoParams();
    pano.shutter();
    return __;
}

/*
 * Menu callback for displaying About... info
 */
int onAboutPanoController(int __){
    display.clearDisplay();
    display.setTextCursor(2, 0);
    display.print("Pano Controller\n\n"
                  "Built " __DATE__ "\n"
                  __TIME__ "\n");
    display.display();
    while (!hid.read());
    hid.clear(4000);
    return __;
}

void onMenuLoop(void){
    displayBatteryStatus();
    display.invertDisplay(display_invert);
    pano.motorsEnable(motors_enable);
}

void loop() {
    displayMenu(menu, display, DISPLAY_ROWS, hid, onMenuLoop);
}
