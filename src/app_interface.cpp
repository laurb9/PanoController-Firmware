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
void callbackUartRX(char data[], uint16_t len){
    //{ debug
        Serial.print("BLE UART RX len=");
        Serial.print(len);
        Serial.print(" data=");
        for (int i = 0; i < len; i++) { Serial.print(data[i], HEX); Serial.print("-"); };
        Serial.println("");
        //} debug
        app.uartRX((uint8_t *)data, len);
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
    ble.sendCommandCheckOK("AT+HWMODELED=BLEUART");
    ble.sendCommandCheckOK("AT+GAPDEVNAME=Pano Controller");
    
    gatt = new Adafruit_BLEGatt(ble);
    service_id = gatt->addService(SERVICE_UUID);
    config_char_id = gatt->addCharacteristic(CONFIG_CHAR_UUID, GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE | GATT_CHARS_PROPERTIES_WRITE_WO_RESP,
                                             5, 20, BLE_DATATYPE_BYTEARRAY, "Configuration");
    status_char_id = gatt->addCharacteristic(STATUS_CHAR_UUID, GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_NOTIFY,
                                             sizeof(PanoState), sizeof(PanoState), BLE_DATATYPE_BYTEARRAY, "Status");
    cmd_char_id = gatt->addCharacteristic(CMD_CHAR_UUID, GATT_CHARS_PROPERTIES_WRITE,
                                          1, 20, BLE_DATATYPE_BYTEARRAY, "Command");
    ble.sendCommandCheckOK("AT+GAPSETADVDATA=07-02-17-20-0A-18-0F-18"); // 7=number of bytes following, 02-flag for 16-bit service uuids, 17-20 = 0x2017, 0x180A, 0x180F
    ble.sendCommandCheckOK("AT+GAPCONNECTABLE=1");
    ble.reset();
    ble.sendCommandCheckOK("AT+GATTLIST");
    ble.setConnectCallback(callbackDidConnect);
    ble.setDisconnectCallback(callbackDidDisconnect);
    ble.setBleGattRxCallback(config_char_id, callbackGattRX);
    ble.setBleGattRxCallback(cmd_char_id, callbackGattRX);
    ble.setBleUartRxCallback(callbackUartRX);
}
    
void AppInterface::sendStatus(){
    //Serial.println("Send status");
    gatt->setChar(status_char_id, (uint8_t*)&state, sizeof(state));
}

void AppInterface::gattRX(int32_t char_id, uint8_t* data, uint16_t len){
    if (char_id == config_char_id || char_id == cmd_char_id){
        uartRX(data, len);
    }
}
static bool unpack(settings_t& variable, uint8_t* &data, uint16_t& len){
    if (len >= 2*sizeof(variable)){
        len -= 2*sizeof(variable);
        variable = 0;
        for (int i=2*sizeof(variable); i; i--){
            variable = (variable <<= 4) + (*data++ - 0x30);
        }
        return true;
    }
    return false;
}
void AppInterface::uartRX(uint8_t* data, uint16_t len){
    static uint8_t keyCode;
    bool updateConfig = false;
    while (len-- >= 1){
        keyCode = *data++;
        switch (keyCode){

            // Configs
            case 0x41: updateConfig |= unpack(settings.focal, data, len); break;
            case 0x42: updateConfig |= unpack(settings.shutter, data, len); break;
            case 0x43: updateConfig |= unpack(settings.pre_shutter, data, len); break;
            case 0x44: updateConfig |= unpack(settings.post_wait, data, len); break;
            case 0x45: updateConfig |= unpack(settings.long_pulse, data, len); break;
            case 0x46: updateConfig |= unpack(settings.aspect, data, len); break;
            case 0x47: updateConfig |= unpack(settings.shots, data, len); break;
            case 0x48: updateConfig |= unpack(settings.motors_enable, data, len); break;
            case 0x49: updateConfig |= unpack(settings.motors_on, data, len); break;
            case 0x4A: updateConfig |= unpack(settings.display_invert, data, len); break;
            case 0x4B: updateConfig |= unpack(settings.horiz, data, len); break;
            case 0x4C: updateConfig |= unpack(settings.vert, data, len); break;

            // Commands
            case 0x61: callbacks.start(); break;
            case 0x62: callbacks.cancel(); break;
            case 0x63: callbacks.pause(); break;
            case 0x64: callbacks.shutter(); break;
            case 0x65: callbacks.setHome(); break;
            case 0x66: callbacks.goHome(); break;
            case 0x67: sendStatus(); break;
            case 0x68: // free move
                if (len >= 2 * 2 * sizeof(settings_t)){
                    settings_t horiz_move, vert_move;
                    unpack(horiz_move, data, len);
                    unpack(vert_move, data, len);
                    callbacks.freeMove(horiz_move/100, vert_move/100);
                }
                break;
            case 0x69: // incremental move (forward or backward)
                if (len >= 1){
                    callbacks.gridMove((char)data[0]);
                    len--;
                    data++;
                }
                break;
            default:
                Serial.print("Unknown code received: "); Serial.println(keyCode, HEX);
        }
    }
    if (updateConfig){
        callbacks.config();
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
