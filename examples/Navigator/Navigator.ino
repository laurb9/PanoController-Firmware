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
#include <BasicStepperDriver.h>
#include "config.h"
#include "pano.h"
#include "camera.h"
#include "hid.h"
#include "joystick.h"
#include "remote.h"
#include "menu.h"
#include "display.h"
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
static PanoSetup* pano;
static Menu* menu;

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

    camera = new Camera(PIN_UNCONNECTED, PIN_UNCONNECTED);
    joystick = new Joystick(JOYSTICK_SW, JOYSTICK_X, JOYSTICK_Y);
    remote = new Remote(REMOTE_IN);
    // HID (Human Interface Device) Combined joystick+remote
    hid = new AllHID(2, new HID* const[2] {joystick, remote});

    pano = new PanoSetup(*camera);

    menu = getMainMenu(settings);
}

/*
 * Send settings to remote execution unit
 */
static void sendSettings(Radio& radio, PanoSettings& settings){
    radio.write_type_data('C', &settings, sizeof(settings));
}
/*
 * Read state from remote execution unit
 */
static bool readState(Radio& radio, PanoState& state){
    uint8_t type = 0;
    uint8_t len = 0;
    if (radio.available()){
        void *buffer;
        len = radio.read_type_data(type, buffer);
        if (len){
            switch(type){

            case 'T': // State information
                memcpy(&state, buffer, sizeof(state));
                setPanoParams();
            }
        }
    }
    return (type != 0);
}

/*
 * Add a status overlay (currently battery and radio status)
 */
void displayStatusOverlay(void){

    // motors active
    if (state.running){
        display.setTextCursor(0,14);
        display.print("*");
    }

    // show a character indicating radio is connected
    display.setTextCursor(0,15);
    if (radio.connected){
        display.print("^");
    } else {
        display.print("X");
    }

    if (!radio.connected){
        return;
    }

    // Print battery voltage at cursor, format is #.#V (4 chars)
    display.setTextCursor(0, 16);
    if (state.battery){
        // poor attempt at blinking
        if (state.battery < 0 && millis() & 1024){
            display.setTextColor(BLACK, WHITE);
        }
        display.printf("%2d.%dV", abs(state.battery)/1000, (abs(state.battery) % 1000)/100);
        display.setTextColor(WHITE, BLACK);
    }
}

/*
 * Display current panorama status (photo index, etc)
 */
void displayPanoStatus(void){
    display.clearDisplay();
    display.setTextCursor(0,0);

    if (state.running){
        display.printf("# %d of %d\n", pano->position+1, pano->getHorizShots()*pano->getVertShots());
        display.printf("at %d x %d\n", 1+pano->getCurRow(), 1+pano->getCurCol());
    } else {
        display.printf("%d photos\n\n", pano->getHorizShots()*pano->getVertShots());
    }
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
    if (state.position + 1 < photos){
        display.printf("%d minutes ", pano->getTimeLeft()/60);
    }
    if (pano->steady_delay_avg > 500){
        display.setTextCursor(6, 16);
        display.printf("%2ds ", (pano->steady_delay_avg+500)/1000);
        display.printf((pano->steady_delay_avg < 8000) ? "\x12" : "!");
    }
    if (state.running){
        display.setTextCursor(7, 0);
        for (int i=(pano->position+1) * DISPLAY_COLS / photos; i > 0; i--){
            display.print('\xdb');
        }
    }
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
    display.printf("grid %d x %d \n", pano->getVertShots(), pano->getHorizShots());
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
 *               if both horiz and vert are NULL then the head moves freely around
 */
bool positionCamera(const char *msg, settings_t *horiz, settings_t *vert){
    struct {
        settings_t horiz_rpm, vert_rpm, horiz_move, vert_move;
        float horiz_offset = 0, vert_offset = 0;
    } move;
    int step = min(camera->getHorizFOV() / 10, 1);

    if (horiz || vert){
        radio.write_type_data('R');  // Set home to current head position
    }

    display.clearDisplay();
    display.setTextCursor(0,0);
    display.println(msg);
    displayArrows();

    while (true){
        hid->read();
        if (hid->isLastEventOk() || hid->isLastEventCancel()) break;

        move.horiz_rpm = DYNAMIC_HORIZ_RPM(camera->getHorizFOV());
        move.horiz_move = joystick->getPositionX() * step;
        if (move.horiz_move == 0){
            if (hid->isLastEventRight()) move.horiz_move = step;
            if (hid->isLastEventLeft()) move.horiz_move = -step;
        }
        if (move.horiz_move && horiz){
            if (move.horiz_move < -move.horiz_offset){
                move.horiz_move = -move.horiz_offset;
            } else if (abs(move.horiz_move + move.horiz_offset) + camera->getHorizFOV() > 360){
                move.horiz_move = 360 - camera->getHorizFOV() - move.horiz_offset;
            }
        }

        move.vert_rpm = DYNAMIC_VERT_RPM(camera->getVertFOV());
        move.vert_move = joystick->getPositionY() * step;
        if (move.vert_move == 0){
            if (hid->isLastEventUp()) move.vert_move = step;
            if (hid->isLastEventDown()) move.vert_move = -step;
            move.vert_rpm /= 3;
        }
        if (move.vert_move && vert){
            if (move.vert_move > -move.vert_offset){
                move.vert_move = -move.vert_offset;
            } else if (-(move.vert_move + move.vert_offset) + camera->getVertFOV() > 180){
                move.vert_move = -(180 - camera->getVertFOV() + move.vert_offset);
            }
        }

        if (vert){
            pano->setFOV((horiz) ? abs(move.horiz_offset) + camera->getHorizFOV() : 360,
                        abs(move.vert_offset) + camera->getVertFOV());
            pano->computeGrid();
            display.setTextCursor(6, 0);
            display.print("          ");
            display.setTextCursor(6, 0);
            displayPanoSize();
            display.printf("FOV %d x %d ", pano->horiz_fov, pano->vert_fov);
            displayStatusOverlay();
            display.display();
        }

        readState(radio, state);
        if (move.horiz_move || move.vert_move){
            radio.write_type_data('M', &move, sizeof(move));
            move.horiz_offset = move.horiz_offset + move.horiz_move;
            move.vert_offset = move.vert_offset + move.vert_move;
        }
    }

    if (horiz || vert){
        if (horiz){
            *horiz = abs(move.horiz_offset) + camera->getHorizFOV();
        }
        if (vert){
            *vert = abs(move.vert_offset) + camera->getVertFOV();
        }

        radio.write_type_data('B');  // Move head back to home position
    }

    return (hid->isLastEventOk());
}

/*
 * Follow panorama execution from start to finish.
 * Button click interrupts.
 */
void followPano(void){
    bool manualMode = (settings.shutter == 0);

    while (state.running){
        displayPanoStatus();

        if (manualMode){
            displayArrows();
        }

        readState(radio, state);

        if (!hid->read()){
            continue;
        }

        // button was clicked mid-pano
        if (!manualMode){
            radio.write_type_data('O');
            manualMode = true;
            hid->clear(1000);
            continue;
        }

        if (hid->isLastEventOk())          radio.write_type_data('S');
        else if (hid->isLastEventCancel()) radio.write_type_data('X');
        else if (hid->isLastEventLeft())   radio.write_type_data('I', "<");
        else if (hid->isLastEventRight())  radio.write_type_data('I', ">");
        else if (hid->isLastEventUp())     radio.write_type_data('I', "^");
        else if (hid->isLastEventDown())   radio.write_type_data('I', "v");
    };

    hid->waitAnyKey();
}

/*
 * Update common camera and pano settings from external vars
 */
void setPanoParams(void){
    sendSettings(radio, settings);
    camera->setAspect(settings.aspect);
    camera->setFocalLength(settings.focal);
    pano->setShutter(settings.shutter, settings.pre_shutter, settings.post_wait, settings.long_pulse);
    pano->setShots(settings.shots);
    pano->setFOV(settings.horiz, settings.vert);
    pano->computeGrid();
    pano->position = state.position;
    pano->steady_delay_avg = state.steady_delay_avg;
}

/*
 * Menu callback invoked by selecting "Repeat"
 */
int onRepeat(int __){
    setPanoParams();
    if (!positionCamera("Adjust start pos\nSet exposure & focus", NULL, NULL)){
        return false;
    }
    menu->sync();

    hid->clear(4000);

    radio.write_type_data('P');    // start Pano
    return __;
}

/*
 * Menu callback invoked by selecting "Start"
 */
int onStart(int __){
    settings_t horiz, vert;

    // set panorama FOV
    if (!positionCamera("Set Top Left", NULL, NULL) ||
        !positionCamera("Set Bottom Right", &horiz, &vert)){
        return false;
    }

    settings.horiz = horiz;
    settings.vert = vert;
    pano->setFOV(horiz, vert);

    return onRepeat(__);
}

/*
 * Menu callback invoked by selecting "360 Pano"
 */
int on360(int __){
    settings_t horiz, vert;
    // set panorama FOV
    horiz = 360;

    if (!positionCamera("Set Top", NULL, NULL) ||
        !positionCamera("Set Bottom", NULL, &vert)){
        return false;
    }

    settings.horiz = horiz;
    settings.vert = vert;
    pano->setFOV(horiz, vert);

    return onRepeat(__);
}

/*
 * Menu callback for displaying last pano info
 */
int onPanoInfo(int __){
    displayPanoInfo();
    hid->waitAnyKey();
    return __;
}

/*
 * Menu callback for testing camera shutter
 */
int onTestShutter(int __){
    radio.write_type_data('S');
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
}

void loop() {
    bool stateChanged;

    sendSettings(radio, settings);
    stateChanged = readState(radio, state);
    /*
     * Render menu or state
     */
    if (state.running){
        followPano();
    } else if (stateChanged){
        setPanoParams();
    }
    displayMenu(*menu, display, DISPLAY_ROWS, *hid, onMenuLoop);

    yield();
}
