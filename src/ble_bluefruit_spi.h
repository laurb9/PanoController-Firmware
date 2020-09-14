#if !defined(ESP32)
/*
 * App Communication over Bluetooth LE Serial
 *
 * Copyright (C)2015-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef BLUETOOTH_H_
#define BLEUTOOTH_H_
 
#include <Adafruit_BluefruitLE_SPI.h>

class Bluetooth {
private:
    Adafruit_BluefruitLE_SPI& ble;
public:
    bool connected = false;
    Bluetooth(Adafruit_BluefruitLE_SPI& ble);
    void begin(void);
    bool isConnected();
    void poll(uint32_t timeout = 0);
};

#endif /* BLUETOOTH_H_ */
#endif // !defined(ESP32)
