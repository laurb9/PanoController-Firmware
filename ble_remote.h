/*
 * Bluetooth LE Remote
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef BLE_REMOTE_H_
#define BLE_REMOTE_H_

#include <Adafruit_BluefruitLE_SPI.h>
#include "hid.h"

class BLERemote : public HID {
private:
    Adafruit_BluefruitLE_SPI& ble;
    uint8_t readButtonCode(void);

public:
    bool active = false;
    BLERemote(Adafruit_BluefruitLE_SPI& ble);
    void init(void);
    unsigned read(void) override;
};

#endif /* BLE_REMOTE_H_ */
