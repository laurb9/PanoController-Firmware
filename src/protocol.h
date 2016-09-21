/*
 * Pano Controller for Arduino project
 * Radio communication
 *
 * Copyright (C)2015,2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <Arduino.h>
#include "radio.h"
#include "pano_settings.h"

typedef struct {
        settings_t horiz_rpm, vert_rpm;
        settings_t horiz_move, vert_move;
        float horiz_offset = 0, vert_offset = 0;
} move_t;

typedef struct {
    void(*config)(void);
    void(*start)(void);
    void(*cancel)(void);
    void(*pause)(void);
    void(*shutter)(void);
    void(*setHome)(void);
    void(*goHome)(void);
    void(*freeMove)(move_t& move);
    void(*gridMove)(char direction);
} comm_callbacks;

/*
 * Commands (Navigator side)
 */
class Command {
protected:
    Radio& radio;
public:
    Command(Radio& radio):radio(radio){};
    void sendConfig(PanoSettings& settings);
    bool getState(PanoState& state, void(*callback)(void)=NULL);
    void setHome(void);
    void goHome(void);
    void freeMove(move_t& move);
    void gridMove(PanoState& state, const char dir);
    void gridMoveUp(PanoState& state){ gridMove(state, '^'); };
    void gridMoveDown(PanoState& state){ gridMove(state, 'v'); };
    void gridMoveLeft(PanoState& state){ gridMove(state, '<'); };
    void gridMoveRight(PanoState& state){ gridMove(state, '>'); };
    void startPano(PanoState& state);
    void pausePano(PanoState& state);
    void continuePano(PanoState& state);
    void cancelPano(PanoState& state);
    void shutter(PanoState& state);
};


/*
 * Execution side
 */
class Exec {
protected:
    Radio& radio;
public:
    Exec(Radio& radio):radio(radio){};
    void sendState(PanoState& state);
    bool getConfig(PanoSettings& settings);
    bool getCmd(PanoSettings& settings, const comm_callbacks& callbacks);
};

#endif /* PROTOCOL_H_ */
