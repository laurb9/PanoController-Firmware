/*
 * Bluetooth 4.0 Pano Controller
 *
 * Pano Controller configured via phone (iOS)
 *
 * Copyright (C)2015-2017 Laurentiu Badea
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
#include "display.h"
#include "mpu.h"
#include "app_interface.h"

// these variables are updated via Bluetooth
PanoSettings settings;

// This is the panorama engine state
// should technically be in the Pano object
PanoState state;

static Display display(OLED_RESET);

static Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
AppInterface app(ble, settings, state);

static Camera camera(CAMERA_FOCUS, CAMERA_SHUTTER);
static MPU mpu(MPU_I2C_ADDRESS, MPU_INT);
static DRV8834 horiz_motor(MOTOR_STEPS, DIR, HORIZ_STEP, nENABLE);
static DRV8834 vert_motor(MOTOR_STEPS, DIR, VERT_STEP);
static Pano pano(horiz_motor, vert_motor, camera, mpu);

void setup() {
    Serial.begin(38400);
    delay(1000); // wait for serial
    Serial.println("PanoController built " __DATE__ " " __TIME__);
    Serial.println("setup()");

    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS, false);
    display.setRotation(0);
    display.clearDisplay();
    display.setTextCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(TEXT_SIZE);

    Serial.println("Checking Camera Shutter Connection");
    pinMode(CAMERA_FOCUS, INPUT_PULLDOWN);
    pinMode(CAMERA_SHUTTER, INPUT_PULLDOWN);
    Serial.print("  Focus: ");
    Serial.println((digitalRead(CAMERA_FOCUS) ? "OK" : "N/C"));
    Serial.print("  Shutter: ");
    Serial.println((digitalRead(CAMERA_SHUTTER) ? "OK" : "N/C"));

    Serial.println("Configuring MPU");
    mpu.begin();

    Serial.println("Configuring stepper drivers");
    horiz_motor.begin(MOTOR_RPM, MICROSTEPS);
    horiz_motor.setSpeedProfile(LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
    vert_motor.begin(MOTOR_RPM, MICROSTEPS);
    vert_motor.setSpeedProfile(LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);

    Serial.println("Configuring Bluefruit LE");
    ble.begin(true);
    ble.echo(false);     // disable command echo
    ble.factoryReset();
    ble.info();
    ble.verbose(false);  // turn off debug info

    Serial.println("Initializing BLE App Communication");
    app.begin();

    ble.sendCommandCheckOK("AT+BLEBATTEN=1"); // enable battery service
    // ble.sendCommandCheckOK("AT+BLEPOWERLEVEL=0"); // can be used to change transmit power

    Serial.print("Checking battery voltage... ");
    // Configure input pin and ADC for reading battery voltage
    pinMode(BATTERY, INPUT);
#if defined(__MK20DX256__) || defined(__MKL26Z64__)
    analogReadRes(10);
    analogReadAveraging(32);
#endif
    readBattery();
    Serial.print(abs(state.battery)); Serial.println("mV");

    Serial.println("Waiting to connect...");
    display.printf("Waiting to connect...");
    display.display();

    while (!app.isConnected()) app.poll(100);

    Serial.println("Connected, waiting for config data");

    pano.begin();
}

void readBattery(void){
    // mV
    state.battery = map(analogRead(BATTERY), 0, (1<<10)-1, 0, BATT_RANGE/100) * 100;
    if (state.battery < LOW_BATTERY){
        state.battery = -state.battery;
        ble.sendCommandCheckOK("AT+BLEBATTVAL=10");  // battery at 10% 
    } else {
        ble.sendCommandCheckOK("AT+BLEBATTVAL=100"); // battery full
    }
}

/*
 * Display current panorama status (photo index, etc)
 */
void displayPanoStatus(bool complete){
    display.clearDisplay();
    display.setTextCursor(0,0);
    int photos = pano.getHorizShots() * pano.getVertShots();

    if (state.running){
        display.printf("# %d of %d\n", pano.position+1, photos);
        display.printf("at %d x %d\n", 1+pano.getCurRow(), 1+pano.getCurCol());
    } else {
        display.printf("%d photos\n\n", photos);
    }
    display.printf("grid %d x %d \n", pano.getVertShots(), pano.getHorizShots());

    float horiz_fov = camera.getHorizFOV();
    float vert_fov = camera.getVertFOV();
    display.setTextCursor(3,0);
    display.printf("Lens: %dmm\n", settings.focal);
    display.printf("      %d.%d x %d.%d\n",
                   int(horiz_fov), round(10*(horiz_fov-int(horiz_fov))),
                   int(vert_fov), round(10*(vert_fov-int(vert_fov))));
    display.printf("Pano %d x %d deg\n", pano.horiz_fov, pano.vert_fov);

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
    if (ble.isConnected()){
        display.print("^");
    } else {
        display.print("X");
    }

    display.setTextCursor(6, 0);
    if (state.position + 1 < photos){
        display.printf("%d minutes ", pano.getTimeLeft()/60);
    }
    if (pano.steady_delay_avg > 500){
        display.setTextCursor(6, 16);
        display.printf("%2ds ", (pano.steady_delay_avg+500)/1000);
        display.printf((pano.steady_delay_avg < 8000) ? "\x12" : "!");
    }

    // show progress bar
    if (state.running){
        display.setTextCursor(7, 0);
        for (int i=(pano.position+1) * DISPLAY_COLS / photos; i > 0; i--){
            display.print('\xdb');
        };
    };
    if (complete){
        display.display();
    }
}

/*
 * Update camera and pano settings
 */
void setPanoParams(void){
    camera.setAspect(settings.aspect);
    camera.setFocalLength(settings.focal);
    pano.setShutter(settings.shutter, settings.pre_shutter, settings.post_wait, settings.long_pulse);
    pano.setShots(settings.shots);
    pano.setFOV(settings.horiz, settings.vert);
    pano.computeGrid();
    // pano.position = state.position;
}

/*
 * Callbacks for commands received from app
 */
void doConfig(void){
    setPanoParams();
    display.invertDisplay(settings.display_invert);
};
void doStart(void){
    Serial.println("Start pano");
    pano.start();
    state.running = true;
    state.paused = (settings.shutter == 0);
};
void doCancel(void){
    Serial.println("Cancel pano");
    pano.end();
    state.running = false;
    state.paused = false;
};
void doPause(void){
    Serial.println("Pause pano");
    if (state.running){
        state.paused = true;
    }
};
void doShutter(void){
    Serial.println("Shutter");
    pano.shutter();
    if (state.running){ // manual shutter mode, advance to next
        state.running = pano.next();
        if (!state.running){
            pano.end();
        }
    }
};
void doSetHome(void){
    Serial.println("Set home");
    pano.setMotorsHomePosition();
};
void doGoHome(void){
    Serial.println("Go home");
    pano.motorsEnable(true);
    pano.moveMotorsHome();
};
void doFreeMove(float horiz_move, float vert_move){
    pano.motorsEnable(true);
    pano.moveMotors(horiz_move, vert_move);
};
void doGridMove(const char direction){
    Serial.println("Inc move");
    switch (direction){
    case '<': pano.prev(); break;
    case '>': pano.next(); break;
    case '^': pano.moveTo(pano.getCurRow() - 1, pano.getCurCol()); break;
    case 'v': pano.moveTo(pano.getCurRow() + 1, pano.getCurCol()); break;
    }
};
// unused
void doGridMoveTo(int16_t row, int16_t col){
    Serial.println("Head move");
    pano.moveTo(row, col);
}

const app_callbacks callbacks = {
    doConfig,
    doStart,
    doCancel,
    doPause,
    doShutter,
    doSetHome,
    doGoHome,
    doFreeMove,
    doGridMove
};

void loop() {
    /*
     * Check if anything was sent to us
     */
    app.poll(20);
    /*
     * Collect and send state to menu navigator
     */
    readBattery();
    state.steady_delay_avg = pano.steady_delay_avg;
    state.position = pano.position;
    state.horiz_offset = pano.horiz_home_offset;
    state.vert_offset = pano.vert_home_offset;
    state.motors_on = settings.motors_on;
    /*
     * Render state.
     * TODO: We should do this only if anything has changed though.
     */
    //displayPanoStatus(true);
    /*
     * Execute pano
     */
    if (ble.isConnected() || state.running){
        if (!settings.motors_enable || state.running){
            app.sendStatus();
            displayPanoStatus(true);
        }
    } else {
        display.clearDisplay();
        display.display();
    }
    if (state.running){
        if (!state.paused){
            pano.shutter();
            state.running = pano.next();
            if (!state.running){
                pano.end();
            };
        };
    } else {
        pano.motorsEnable(state.motors_on);
    };

    yield();
}
