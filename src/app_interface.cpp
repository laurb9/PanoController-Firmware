/*
 * App Communication over Bluetooth LE using GATT Profile
 *
 * Copyright (C)2015-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "app_interface.h"

extern AppInterface app;

AppInterface::AppInterface(Adafruit_BluefruitLE_SPI& ble, PanoSettings& settings, PanoState& state)
:settings(settings), state(state), ble(ble)
{}

/*
 * Set up some redirects from BLE callbacks in global scope to app singleton methods 
 */
void callbackGattRX(int32_t char_id, uint8_t* data, uint16_t len){
    //{ debug
    Serial.print("BLE GATT RX char=");
    Serial.print(char_id);
    Serial.print(" len=");
    Serial.print(len);
    Serial.print(" data=");
    for (int i = 0; i < len; i++) { Serial.print(data[i], HEX); Serial.print("-"); };
    Serial.println("");
    //} debug
    app.gattRX(char_id, data, len);
}
void callbackDidConnect(void){
    app.didConnect();
}
void callbackDidDisconnect(void){
    app.didDisconnect();
}

void AppInterface::begin(){
    ble.sendCommandCheckOK("AT+BAUDRATE=921600");
    // LED Activity command is only supported from 0.6.6
    ble.sendCommandCheckOK("AT+HWMODELED=SPI");
    ble.sendCommandCheckOK("AT+GAPDEVNAME=Pano Controller");
    
    gatt = new Adafruit_BLEGatt(ble);
    service_id = gatt->addService(SERVICE_UUID);
    config_char_id = gatt->addCharacteristic(CONFIG_CHAR_UUID, GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE | GATT_CHARS_PROPERTIES_WRITE_WO_RESP,
                                             3, sizeof(PanoSettings), BLE_DATATYPE_BYTEARRAY, "Configuration");
    status_char_id = gatt->addCharacteristic(STATUS_CHAR_UUID, GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_NOTIFY,
                                             sizeof(PanoState), sizeof(PanoState), BLE_DATATYPE_BYTEARRAY, "Status");
    cmd_char_id = gatt->addCharacteristic(CMD_CHAR_UUID, GATT_CHARS_PROPERTIES_WRITE,
                                          1, 5, BLE_DATATYPE_BYTEARRAY, "Command");
    ble.sendCommandCheckOK("AT+GAPSETADVDATA=07-02-17-20-0A-18-0F-18"); // 7=number of bytes following, 02-flag for 16-bit service uuids, 17-20 = 0x2017, 0x180A, 0x180F
    ble.sendCommandCheckOK("AT+GAPCONNECTABLE=1");
    ble.reset();
    ble.sendCommandCheckOK("AT+GATTLIST");
    ble.setConnectCallback(callbackDidConnect);
    ble.setDisconnectCallback(callbackDidDisconnect);
    ble.setBleGattRxCallback(config_char_id, callbackGattRX);
    ble.setBleGattRxCallback(cmd_char_id, callbackGattRX);
}
    
void AppInterface::sendStatus(){
    //Serial.println("Send status");
    gatt->setChar(status_char_id, (uint8_t*)&state, sizeof(state));
}

void AppInterface::gattRX(int32_t char_id, uint8_t* data, uint16_t len){
    if (char_id = config_char_id){  // Configuration setting(s) received
        while (len >= 3){
            len -= 3;
            uint8_t keyCode = data[0];
            settings_t value = data[1] + 256*data[2];
            switch (keyCode){
                case 0x41: settings.focal = value; break;
                case 0x42: settings.shutter = value; break;
                case 0x43: settings.pre_shutter = value; break;
                case 0x44: settings.post_wait = value; break;
                case 0x45: settings.long_pulse = value; break;
                case 0x46: settings.aspect = value; break;
                case 0x47: settings.shots = value; break;
                case 0x48: settings.motors_enable = value; break;
                case 0x49: settings.motors_on = value; break;
                case 0x50: settings.display_invert = value; break;
                case 0x51: settings.horiz = value; break;
                case 0x52: settings.vert = value; break;
                default:
                    Serial.print("Unknown config code "); Serial.println(keyCode, HEX);
            }
            callbacks.config();
        }
    } else if (char_id == cmd_char_id){ // Command received
        uint8_t keyCode = data[0];
        move_t move;
        switch (keyCode){
            case 0x61: callbacks.start(); break;
            case 0x62: callbacks.cancel(); break;
            case 0x63: callbacks.pause(); break;
            case 0x64: callbacks.shutter(); break;
            case 0x65: callbacks.setHome(); break;
            case 0x66: callbacks.goHome(); break;
            case 0x67: sendStatus(); break;
            case 0x68: // free move
                // TODO: check that len=5
                memcpy(&move, data, sizeof(move));
                callbacks.freeMove(move);
                break;
            case 0x69: // incremental move (forward or backward)
                // TODO: check that len=2
                callbacks.gridMove((char)data[1]);
                break;
            default:
                Serial.print("Unknown command code "); Serial.println(keyCode, HEX);
        }
    }
}

void AppInterface::didConnect(void){
    Serial.println("BLE connected");
    connected = true;
}

void AppInterface::didDisconnect(void){
    Serial.println("BLE disconnected");
    connected = false;
}

bool AppInterface::isConnected(void){
    return connected;
}

void AppInterface::poll(uint32_t timeout){
    ble.update(timeout);
}
