/*
 * App Communication over Bluetooth LE using GATT Profile
 *
 * Copyright (C)2015-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef APP_BLE_H_
#define APP_BLE_H_

#include <Adafruit_BluefruitLE_SPI.h>
#include <Adafruit_BLEGatt.h>
#include "pano_settings.h"

typedef struct {
    void(*config)(void);
    void(*start)(void);
    void(*cancel)(void);
    void(*pause)(void);
    void(*shutter)(void);
    void(*setHome)(void);
    void(*goHome)(void);
    void(*freeMove)(float horiz, float vert);
    void(*gridMove)(char direction);
} app_callbacks;

extern const app_callbacks callbacks;  // this is because the GATT callback itself is static

class AppInterface {
private:
    static const uint16_t SERVICE_UUID = 0x2017;
    static const uint16_t CONFIG_CHAR_UUID = 0x0001;
    static const uint16_t STATUS_CHAR_UUID = 0x0002;
    static const uint16_t CMD_CHAR_UUID = 0x0003;
    PanoSettings& settings;
    PanoState& state;
    Adafruit_BluefruitLE_SPI& ble;
    Adafruit_BLEGatt* gatt;
    uint8_t service_id, config_char_id, status_char_id, cmd_char_id;
public:
    bool connected = false;
    AppInterface(Adafruit_BluefruitLE_SPI& ble, PanoSettings& settings, PanoState& state);
    void begin(void);
    void sendStatus();
    bool isConnected();
    void poll(uint32_t timeout = 0);

    void gattRX(int32_t char_id, uint8_t* data, uint16_t len);
    void uartRX(uint8_t* data, uint16_t len);
    void didConnect(void);
    void didDisconnect(void);
};

#endif /* APP_BLE_H_ */
