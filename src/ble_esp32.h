#if defined(ESP32)

#ifndef BLE_ESP32_H
#define BLE_ESP32_H

#include <Stream.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define FIFO_SIZE 256

class ServerCallbacks : public BLEServerCallbacks {
private:
    volatile bool deviceConnected;

public:
    void onConnect(BLEServer *pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer) {
        deviceConnected = false;
        pServer->startAdvertising();
    };

    bool isConnected(void) {
        return deviceConnected;
    }
};

class RXCallbacks : public BLECharacteristicCallbacks {
private:
    uint8_t buf[FIFO_SIZE];
    uint8_t *readAt;
    uint8_t *writeAt;
public:
    RXCallbacks()
    {
        flush();
    }
    void onWrite(BLECharacteristic *pCharacteristic);

    bool available(void) {
        return readAt != writeAt;
    }
    // Class Stream interface
    int read(void);
    int peek(void);
    void flush(void);

};


class Bluetooth : public Stream {
private:
    BLEServer *pServer;
    BLECharacteristic *tx, *rx;
    ServerCallbacks serverCallbacks;
    RXCallbacks rxCallbacks;
    uint8_t txValue = 0;
public:
    Bluetooth() {};
    void begin(const char *name);
    bool isConnected();


    // Class Print virtual function Interface
    virtual size_t write(uint8_t c);
    virtual size_t write(const uint8_t *buffer, size_t size);

    // pull in write(str) and write(buf, size) from Print
    using Print::write;

    // Class Stream interface
    virtual int available(void);
    virtual int read(void);
    virtual void flush(void);
    virtual int peek(void);
};

#endif /* BLE_ESP32_H */

#endif // defined(ESP32)
