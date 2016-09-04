/*
 * Pano Controller for Arduino project
 *
 * Copyright (C)2015,2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>
#include <SPI.h>
#include <DRV8834.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include "config.h"
#include "pano.h"
#include "camera.h"
#include "hid.h"
#include "joystick.h"
#include "remote.h"
#include "ble_remote.h"
#include "menu.h"
#include "display.h"
#include "mpu.h"

// these variables are modified by the menu
PanoSettings settings;

static Display display(OLED_RESET);
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

static Camera* camera;
static Joystick* joystick;
static Remote* remote;
static BLERemote* ble_remote;
// HID (Human Interface Device) Combined joystick+remote
static AllHID* hid;
static MPU* mpu;
static DRV8834* horiz_motor;
static DRV8834* vert_motor;
static Pano* pano;
static Menu* menu;

void setup() {
    Serial.begin(38400);
    delay(1000); // wait for serial

    ble.begin(true);
    ble.echo(false);     // disable command echo
    ble.factoryReset();
    ble.info();
    ble.verbose(false);  // turn off debug info

    // LED Activity command is only supported from 0.6.6
    ble.sendCommandCheckOK("AT+HWModeLED=BLEUART");
    ble.sendCommandCheckOK("AT+GAPDEVNAME=Pano Controller");

    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS, false);
    //display.setRotation(2);
    display.clearDisplay();
    display.setTextCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(TEXT_SIZE);

    camera = new Camera(CAMERA_FOCUS, CAMERA_SHUTTER);
    joystick = new Joystick(JOYSTICK_SW, JOYSTICK_X, JOYSTICK_Y);
    remote = new Remote(REMOTE_IN);
    ble_remote = new BLERemote(ble);
    // HID (Human Interface Device) Combined joystick+remote
    hid = new AllHID(3, new HID* const[3] {joystick, remote, ble_remote});

    mpu = new MPU(MPU_I2C_ADDRESS, MPU_INT);
    mpu->init();

    horiz_motor = new DRV8834(MOTOR_STEPS, DIR, HORIZ_STEP, nENABLE);
    vert_motor = new DRV8834(MOTOR_STEPS, DIR, VERT_STEP);
    horiz_motor->setMicrostep(32);
    vert_motor->setMicrostep(32);

    pano = new Pano(*horiz_motor, *vert_motor, *camera, *mpu);

    //pinMode(COMPASS_DRDY, INPUT_PULLUP);

    pinMode(BATTERY, INPUT);
#if defined(__MK20DX256__) || defined(__MKL26Z64__)
    analogReadRes(10);
    analogReadAveraging(32);
#endif

    menu = getMainMenu(settings);
}

int readBattery(void){
    return map(analogRead(BATTERY), 0, (1<<10)-1, 0, BATT_RANGE);
}

/*
 * Add a status overlay (currently battery and bluetooth status)
 */
void displayStatusOverlay(void){

    // Print battery voltage at cursor, format is #.#V (4 chars)
    int battmV = readBattery();
    display.setTextCursor(0, 16);
    // poor attempt at blinking
    if (battmV < LOW_BATTERY && millis() & 1024){
        display.setTextColor(BLACK, WHITE);
    }
    display.printf("%2d.%dV", battmV/1000, (battmV % 1000)/100);
    display.setTextColor(WHITE, BLACK);

    // show a character indicating bluetooth is connected
    if (ble_remote->active){
        display.setTextCursor(0,15);
        display.print("\xe8");
    }
}

/*
 * Display current panorama status (photo index, etc)
 */
void displayPanoStatus(void){
    display.clearDisplay();
    display.setTextCursor(0,0);

    display.printf("Photo %d of %d\n", pano->position+1, pano->getHorizShots()*pano->getVertShots());
    display.printf("At %d x %d\n", 1+pano->getCurRow(), 1+pano->getCurCol());
    displayPanoSize();
    displayStatusOverlay();
    displayProgress();
    display.display();
}
/*
 * Display progress information (minutes / progress bar)
 */
void displayProgress(void){
    int photos = pano->getHorizShots() * pano->getVertShots();
    display.setTextCursor(6, 0);
    display.printf("%d minutes ", pano->getTimeLeft()/60);
    if (pano->steady_delay_avg > 500){
        display.setTextCursor(6, 16);
        display.printf("%2ds ", (pano->steady_delay_avg+500)/1000);
        display.printf((pano->steady_delay_avg < 8000) ? "\x12" : "!");
    }
    display.setTextCursor(7, 0);
    for (int i=(pano->position+1) * DISPLAY_COLS / photos; i > 0; i--){
        display.print('\xda');
    }
    display.println();
}
/*
 * Display panorama information
 */
void displayPanoInfo(void){
    float horiz_fov = camera->getHorizFOV();
    float vert_fov = camera->getVertFOV();
    display.clearDisplay();
    display.setTextCursor(0,0);
    display.printf("Lens: %dmm\n", settings.focal);
    display.printf("      %d.%d x %d.%d\n",
                   int(horiz_fov), round(10*(horiz_fov-int(horiz_fov))),
                   int(vert_fov), round(10*(vert_fov-int(vert_fov))));
    display.printf("Pano FOV %d x %d \n", pano->horiz_fov, pano->vert_fov);
    displayPanoSize();
    display.printf("%d photos\n", pano->getHorizShots()*pano->getVertShots());
    displayProgress();
    displayStatusOverlay();
    display.display();
}

/*
 * Display the panorama grid size
 */
void displayPanoSize(){
    display.printf("Grid %d x %d \n", pano->getVertShots(), pano->getHorizShots());
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
bool positionCamera(const char *msg, settings_t *horiz, settings_t *vert){
    int pos_x, pos_y;
    int horiz_rpm, vert_rpm;

    display.clearDisplay();
    display.setTextCursor(0,0);
    display.println(msg);
    displayArrows();
    pano->motorsEnable(true);

    if (horiz || vert){
        pano->setMotorsHomePosition();
    }

    while (true){
        hid->read();
        if (hid->isLastEventOk() || hid->isLastEventCancel()) break;

        pos_x = joystick->getPositionX();
        horiz_rpm = DYNAMIC_HORIZ_RPM(camera->getHorizFOV());
        if (pos_x == 0){
            if (hid->isLastEventRight()) pos_x = 1;
            if (hid->isLastEventLeft()) pos_x = -1;
            horiz_rpm /= 3;
        } else {
            // proportional speed control
            horiz_rpm = horiz_rpm * abs(pos_x) / joystick->range;
            pos_x = pos_x / abs(pos_x);
        }
        if (pos_x && horiz){
            if (pos_x < -pano->horiz_home_offset){
                pos_x = -pano->horiz_home_offset;
            } else if (abs(pos_x + pano->horiz_home_offset) + camera->getHorizFOV() > 360){
                pos_x = 360 - camera->getHorizFOV() - pano->horiz_home_offset;
            }
        }

        pos_y = joystick->getPositionY();
        vert_rpm = DYNAMIC_VERT_RPM(camera->getVertFOV());
        if (pos_y == 0){
            if (hid->isLastEventUp()) pos_y = 1;
            if (hid->isLastEventDown()) pos_y = -1;
            vert_rpm /= 3;
        } else {
            // proportional speed control
            vert_rpm = vert_rpm * abs(pos_y) / joystick->range;
            pos_y = pos_y / abs(pos_y);
        }
        if (pos_y && vert){
            if (pos_y > -pano->vert_home_offset){
                pos_y = -pano->vert_home_offset;
            } else if (-(pos_y + pano->vert_home_offset) + camera->getVertFOV() > 180){
                pos_y = -(180 - camera->getVertFOV() + pano->vert_home_offset);
            }
        }

        if (vert && !pos_x && !pos_y){
            pano->setFOV((horiz) ? abs(pano->horiz_home_offset) + camera->getHorizFOV() : 360,
                        abs(pano->vert_home_offset) + camera->getVertFOV());
            pano->computeGrid();
            display.setTextCursor(6, 0);
            display.print("          ");
            display.setTextCursor(6, 0);
            displayPanoSize();
            display.printf("FOV %d x %d ", pano->horiz_fov, pano->vert_fov);
            displayStatusOverlay();
            display.display();
        }

        if (pos_x || pos_y){
            horiz_motor->setRPM(horiz_rpm);
            vert_motor->setRPM(vert_rpm);
            pano->moveMotors(pos_x, pos_y);
        }
    }

    if (horiz || vert){
        if (horiz){
            *horiz = abs(pano->horiz_home_offset) + camera->getHorizFOV();
        }
        if (vert){
            *vert = abs(pano->vert_home_offset) + camera->getVertFOV();
        }
        pano->moveMotorsHome();
    }
    return (hid->isLastEventOk());
}

/*
 * Execute panorama from start to finish.
 * Button click interrupts.
 */
void executePano(void){

    hid->clear(4000);
    pano->start();

    while (settings.running){
        displayPanoStatus();
        if (!pano->position){
            delay(2000);
        }
        if (settings.shutter > 0){
            pano->shutter();
        }

        if (settings.shutter == 0 || hid->read()){
            hid->clear(1000);
            // button was clicked mid-pano or we are in manual shutter mode
            displayPanoStatus();
            displayArrows();
            while (settings.running){
                if (!hid->read()) continue;
                else if (hid->isLastEventLeft()) pano->prev();
                else if (hid->isLastEventRight()) pano->next();
                else if (hid->isLastEventUp()) pano->moveTo(pano->getCurRow() - 1, pano->getCurCol());
                else if (hid->isLastEventDown()) pano->moveTo(pano->getCurRow() + 1, pano->getCurCol());
                else if (hid->isLastEventOk()) break;
                else if (hid->isLastEventCancel()){
                    settings.running = false;
                    break;
                }
                displayPanoStatus();
                displayArrows();
            }
        }
        settings.running = settings.running && pano->next();
    };

    settings.running = false;
    displayPanoStatus();

    pano->end();

    hid->waitAnyKey();
}

/*
 * Update common camera and pano settings from external vars
 */
void setPanoParams(void){
    menu->cancel();
    camera->setAspect(settings.aspect);
    camera->setFocalLength(settings.focal);
    pano->setShutter(settings.shutter, settings.pre_shutter, settings.post_wait, settings.long_pulse);
    pano->setShots(settings.shots);
}

/*
 * Menu callback invoked by selecting "Start"
 */
int onStart(int __){
    setPanoParams();

    // set panorama FOV
    if (!positionCamera("Set Top Left", NULL, NULL) ||
        !positionCamera("Set Bottom Right", &settings.horiz, &settings.vert)){
        return false;
    }

    pano->setFOV(settings.horiz, settings.vert);
    if (!positionCamera("Adjust start pos\nSet exposure & focus", NULL, NULL)){
        return false;
    }
    settings.running = true;
    menu->sync();
    executePano();
    return __;
}

/*
 * Menu callback invoked by selecting "Repeat"
 */
int onRepeat(int __){
    setPanoParams();

    pano->setFOV(settings.horiz, settings.vert);
    if (!positionCamera("Adjust start pos\nSet exposure & focus", NULL, NULL)){
        return false;
    }
    settings.running = true;
    menu->sync();
    executePano();
    return __;
}

/*
 * Menu callback invoked by selecting "Repeat"
 */
int on360(int __){
    setPanoParams();

    // set panorama FOV
    settings.horiz = 360;
    if (!positionCamera("Set Top", NULL, NULL) ||
        !positionCamera("Set Bottom", NULL, &settings.vert)){
        return false;
    }

    pano->setFOV(settings.horiz, settings.vert);
    if (!positionCamera("Adjust start pos\nSet exposure & focus", NULL, NULL)){
        return false;
    }
    settings.running = true;
    menu->sync();
    executePano();
    return __;
}

/*
 * Menu callback for displaying last pano info
 */
int onPanoInfo(int __){
    setPanoParams();
    pano->setFOV(settings.horiz, settings.vert);
    pano->computeGrid();
    displayPanoInfo();
    hid->waitAnyKey();
    return __;
}

/*
 * Menu callback for testing camera shutter
 */
int onTestShutter(int __){
    setPanoParams();
    pano->shutter();
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
    hid->waitAnyKey();
    return __;
}

void onMenuLoop(void){
    displayStatusOverlay();
    display.invertDisplay(settings.display_invert);
    pano->motorsEnable(settings.motors_enable);
}

void loop() {
    displayMenu(*menu, display, DISPLAY_ROWS, *hid, onMenuLoop);
}
