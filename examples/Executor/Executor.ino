/*
 * Headless Pano Controller
 *
 * A basic panorama controller without menus and advanced HID devices.
 *
 * Copyright (C)2015,2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>
#include <SPI.h>
#include <DRV8834.h>
#include "config.h"
#include "pano.h"
#include "camera.h"
#include "hid.h"
#include "joystick.h"
#include "remote.h"
#include "display.h"
#include "mpu.h"
#include "radio.h"

// these variables are modified by the menu
PanoSettings settings;

// This is the panorama engine state
// should technically be in the Pano object
PanoState state;

static Display display(OLED_RESET);
static Radio radio(NRF24_CE, NRF24_CSN);

static Camera* camera;
static Joystick* joystick;
static Remote* remote;
// HID (Human Interface Device) Combined joystick+remote
static AllHID* hid;
static MPU* mpu;
static DRV8834* horiz_motor;
static DRV8834* vert_motor;
static Pano* pano;

void setup() {
    Serial.begin(38400);
    delay(1000); // wait for serial

    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS, false);
    //display.setRotation(2);
    display.clearDisplay();
    display.setTextCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(TEXT_SIZE);

    radio.begin();

    camera = new Camera(CAMERA_FOCUS, CAMERA_SHUTTER);
    joystick = new Joystick(JOYSTICK_SW, JOYSTICK_X, JOYSTICK_Y);
    remote = new Remote(REMOTE_IN);
    // HID (Human Interface Device) Combined joystick+remote
    hid = new AllHID(2, new HID* const[2] {joystick, remote});

    mpu = new MPU(MPU_I2C_ADDRESS, MPU_INT);
    mpu->init();

    horiz_motor = new DRV8834(MOTOR_STEPS, DIR, HORIZ_STEP, nENABLE);
    vert_motor = new DRV8834(MOTOR_STEPS, DIR, VERT_STEP);
    horiz_motor->setMicrostep(32);
    vert_motor->setMicrostep(32);

    pano = new Pano(*horiz_motor, *vert_motor, *camera, *mpu);

    pinMode(BATTERY, INPUT);
#if defined(__MK20DX256__) || defined(__MKL26Z64__)
    analogReadRes(10);
    analogReadAveraging(32);
#endif
}

void readBattery(void){
    state.battery = map(analogRead(BATTERY), 0, (1<<10)-1, 0, BATT_RANGE);
    if (state.battery < LOW_BATTERY){
        state.battery = -state.battery;
    }
}

/*
 * Display current panorama status (photo index, etc)
 */
void displayPanoStatus(bool complete){
    display.clearDisplay();
    display.setTextCursor(0,0);
    int photos = pano->getHorizShots() * pano->getVertShots();

    if (state.running){
        display.printf("# %d of %d\n", pano->position+1, photos);
        display.printf("at %d x %d\n", 1+pano->getCurRow(), 1+pano->getCurCol());
    } else {
        display.printf("%d photos\n\n", photos);
    }
    display.printf("grid %d x %d \n", pano->getVertShots(), pano->getHorizShots());

    float horiz_fov = camera->getHorizFOV();
    float vert_fov = camera->getVertFOV();
    display.setTextCursor(3,0);
    display.printf("Lens: %dmm\n", settings.focal);
    display.printf("      %d.%d x %d.%d\n",
                   int(horiz_fov), round(10*(horiz_fov-int(horiz_fov))),
                   int(vert_fov), round(10*(vert_fov-int(vert_fov))));
    display.printf("Pano %d x %d deg\n", pano->horiz_fov, pano->vert_fov);

    // Print battery voltage at cursor, format is #.#V (4 chars)
    display.setTextCursor(0, 16);
    // poor attempt at blinking
    if (state.battery < 0 && millis() & 1024){
        display.setTextColor(BLACK, WHITE);
    }
    display.printf("%2d.%dV", abs(state.battery)/1000, (abs(state.battery) % 1000)/100);
    display.setTextColor(WHITE, BLACK);

    // motors active
    if (state.running){
        display.setTextCursor(0,14);
        display.print("*");
    }

    // radio connection status
    display.setTextCursor(0,15);
    if (radio.connected){
        display.print("^");
    } else {
        display.print("X");
    }

    display.setTextCursor(6, 0);
    if (state.position + 1 < photos){
        display.printf("%d minutes ", pano->getTimeLeft()/60);
    }
    if (pano->steady_delay_avg > 500){
        display.setTextCursor(6, 16);
        display.printf("%2ds ", (pano->steady_delay_avg+500)/1000);
        display.printf((pano->steady_delay_avg < 8000) ? "\x12" : "!");
    }

    // show progress bar
    if (state.running){
        display.setTextCursor(7, 0);
        for (int i=(pano->position+1) * DISPLAY_COLS / photos; i > 0; i--){
            display.print('\xdb');
        };
    };
    if (complete){
        display.display();
    }
}

/*
 * Update camera and pano settings received from navigator
 */
void setPanoParams(void){
    camera->setAspect(settings.aspect);
    camera->setFocalLength(settings.focal);
    pano->setShutter(settings.shutter, settings.pre_shutter, settings.post_wait, settings.long_pulse);
    pano->setShots(settings.shots);
    pano->setFOV(settings.horiz, settings.vert);
    pano->computeGrid();
    // pano->position = state.position;
}

void loop() {
    /*
     * Read state from menu navigator
     */
    if (radio.available()){
        uint8_t type = 0;
        uint8_t len = 0;
        void *buffer;
        len = radio.read_type_data(type, buffer);
        if (type != int('C')){
            Serial.print("msg "); Serial.print((char)type); Serial.print(" len "); Serial.println(len);
        }
        switch(type){

        case 'C':   // Configuration settings
            memcpy(&settings, buffer, sizeof(settings));
            setPanoParams();
            display.invertDisplay(settings.display_invert);
            break;

        case 'P':   // Execute pano
            Serial.println("Start pano");
            pano->start();
            state.running = true;
            break;

        case 'X':   // Cancel pano
            Serial.println("Cancel pano");
            pano->end();
            state.running = false;
            break;

        case 'O':   // Pause pano
            Serial.println("Pause pano");
            settings.shutter = 0;
            break;

        case 'S':   // Shutter
            Serial.println("Shutter");
            pano->shutter();
            if (state.running){ // manual shutter mode, advance to next
                state.running = pano->next();
                if (!state.running){
                    pano->end();
                }
            }
            break;

        case 'R':   // Set home to current head position
            Serial.println("Set home");
            pano->setMotorsHomePosition();
            break;

        case 'B':   // Move head to home position
            Serial.println("Go home");
            pano->moveMotorsHome();
            break;

        case 'M':   // Move motors
            Serial.println("Motor move");
            struct {
                settings_t horiz_rpm, vert_rpm, horiz_move, vert_move;
            } motor_move;
            memcpy(&motor_move, buffer, sizeof(motor_move));
            horiz_motor->setRPM(motor_move.horiz_rpm);
            vert_motor->setRPM(motor_move.vert_rpm);
            pano->motorsEnable(true);
            pano->moveMotors(motor_move.horiz_move, motor_move.vert_move);
            break;

        case 'H':   // Move head to requested row/col
            Serial.println("Head move");
            struct {
                settings_t row, col;
            } head_move;
            memcpy(&head_move, buffer, sizeof(head_move));
            pano->moveTo(head_move.row, head_move.col);
            break;

        case 'I':   // Incrementally move head
            Serial.println("Inc move");
            switch (*(char*)buffer){
            case '<': pano->prev(); break;
            case '>': pano->next(); break;
            case '^': pano->moveTo(pano->getCurRow() - 1, pano->getCurCol()); break;
            case 'v': pano->moveTo(pano->getCurRow() + 1, pano->getCurCol()); break;
                break;
            }
        }
    }
    /*
     * Collect and send state to menu navigator
     */
    readBattery();
    state.steady_delay_avg = pano->steady_delay_avg;
    state.position = pano->position;
    state.horiz_offset = pano->horiz_home_offset;
    state.vert_offset = pano->vert_home_offset;
    state.motors_enable = settings.motors_enable;
    radio.write_type_data('T', &state, sizeof(state));
    /*
     * Render state.
     * TODO: We should do this only if anything has changed though.
     */
    //displayPanoStatus(true);
    /*
     * Execute pano
     */
    if (!settings.motors_enable || state.running){
        displayPanoStatus(true);
    }
    if (state.running){
        if (settings.shutter > 0){
            pano->shutter();
            state.running = pano->next();
        };
        if (!state.running){
            pano->end();
        }
    } else {
        pano->motorsEnable(settings.motors_enable);
    };

    yield();
}
