#if !defined(ESP32)
/*
 * App Communication over Bluetooth LE Serial
 *
 * Copyright (C)2015-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "ble_bluefruit_spi.h"

#define UART_BUFFER_SIZE BLE_BUFSIZE

Bluetooth::Bluetooth(Adafruit_BluefruitLE_SPI& ble)
:ble(ble)
{}

void Bluetooth::begin(){
    ble.begin(true);
    ble.echo(false);     // disable command echo
    ble.factoryReset();
    ble.info();
    ble.verbose(false);  // turn off debug info
    ble.sendCommandCheckOK("AT+BLEBATTEN=1"); // enable battery service
    // ble.sendCommandCheckOK("AT+BLEPOWERLEVEL=0"); // can be used to change transmit power
    ble.sendCommandCheckOK("AT+BAUDRATE=921600");
    // LED Activity command is only supported from 0.6.6: MODE, BLEUART
    ble.sendCommandCheckOK("AT+HWMODELED=BLEUART");
    ble.sendCommandCheckOK("AT+GAPDEVNAME=PanoController");
    ble.sendCommandCheckOK("AT+GAPSETADVDATA=07-02-17-20-0A-18-0F-18"); // 7=number of bytes following, 02-flag for 16-bit service uuids, 17-20 = 0x2017, 0x180A, 0x180F
    ble.sendCommandCheckOK("AT+GAPCONNECTABLE=1");
    ble.reset();
    ble.setMode(BLUEFRUIT_MODE_DATA);
}

void Bluetooth::poll(uint32_t timeout){
    static char buffer[UART_BUFFER_SIZE];
    static char* eob = buffer + UART_BUFFER_SIZE;
    if (!connected || timeout > 0){
        ble.update(timeout);
    } else {
        char* p = buffer;
        int c;
        do {
            do {
                // ble.read() can substitute for ble.available() because it can return EOF
                c = ble.read();
                if (c == EOF) break;
                *p++ = (char)c;
            } while (p < eob);

            if (p != buffer){
                // callbackUartRX(buffer, p-buffer);
            }
        } while (p == eob); // we filled the buffer so there may be more to read
    }
}
#endif // !defined(ESP32)
