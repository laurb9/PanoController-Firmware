/*
 * Pano Controller for Arduino project
 * Radio communication
 *
 * Copyright (C)2015,2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "protocol.h"

// Packet types
static const char CMD_CONFIG    = 'C',
           CMD_START     = 'P',
           CMD_CANCEL    = 'X',
           CMD_PAUSE     = 'O',
           CMD_SHUTTER   = 'S',
           CMD_CONTINUE  = 'N',
           CMD_HOME_SET  = 'R',
           CMD_HOME_MOVE = 'B',
           CMD_FREE_MOVE = 'F',
           CMD_GRID_INC  = 'I',
           CMD_STATE     = 'T';

/*
 * Send current state, does not wait for confirmation
 */
void Exec::sendState(PanoState& state){
    radio.write_type_data(CMD_STATE, &state, sizeof(state));
}

/*
 * Receive and execute a command
 * Returns true if a command was received / executed,
 *         false otherwise
 */
bool Exec::getCmd(PanoSettings& settings, const comm_callbacks& callbacks){
    uint8_t type = 0;
    uint8_t len = 0;
    void *buffer = NULL;
    move_t move;
    while (radio.available()){
        len = radio.read_type_data(type, buffer);
        switch((char)type){

        case CMD_CONFIG:   // Configuration settings
            memcpy(&settings, buffer, sizeof(PanoSettings));
            callbacks.config();
            break;

        case CMD_START:
            callbacks.start();
            break;

        case CMD_CANCEL:
            callbacks.cancel();
            break;

        case CMD_PAUSE:
            callbacks.pause();
            break;

        case CMD_SHUTTER:
            callbacks.shutter();
            break;

        case CMD_HOME_SET:
            callbacks.setHome();
            break;

        case CMD_HOME_MOVE:
            callbacks.goHome();
            break;

        case CMD_FREE_MOVE:
            memcpy(&move, buffer, sizeof(move));
            callbacks.freeMove(move);
            break;

        case CMD_GRID_INC:
            callbacks.gridMove(*(char*)buffer);
            break;
        }
    }
    return (buffer != NULL);
}

/*
 * Receive state
 * Returns true if state was modified,
 *         false if it was unmodified or nothing was received
 */
bool Command::getState(PanoState& state, void(*callback)(void)){
    uint8_t type = 0;
    uint8_t len = 0;
    void *buffer;
    while (radio.available()){
        len = radio.read_type_data(type, buffer);
        if (type == CMD_STATE && len == sizeof(state)){
            if (memcmp(&state, buffer, sizeof(state)) == 0){
                // state received, but no change
                return false;
            } else {
                memcpy(&state, buffer, sizeof(state));
                if (callback){
                    callback();
                }
                return true;
            }
        }
    }
    return false;
}

void Command::sendConfig(PanoSettings& settings){
    radio.write_type_data(CMD_CONFIG, &settings, sizeof(settings));
}

// Set home to current head position
void Command::setHome(void){
    radio.write_type_data(CMD_HOME_SET);
}

// Move head to home position
void Command::goHome(void){
    radio.write_type_data(CMD_HOME_MOVE);
}

// Move head in degrees
void Command::freeMove(move_t& move){
    radio.write_type_data(CMD_FREE_MOVE, &move, sizeof(move));
}

void Command::gridMove(PanoState& state, const char dir){
    radio.write_type_data(CMD_GRID_INC, &dir, 1);
}

void Command::startPano(PanoState& state){
    radio.write_type_data(CMD_START);
}

void Command::pausePano(PanoState& state){
    radio.write_type_data(CMD_PAUSE);
}

void Command::cancelPano(PanoState& state){
    radio.write_type_data(CMD_CANCEL);
}

void Command::continuePano(PanoState& state){
    radio.write_type_data(CMD_CONTINUE);
}

void Command::shutter(PanoState& state){
    radio.write_type_data(CMD_SHUTTER);
}
