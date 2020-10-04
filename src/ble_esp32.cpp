#if defined(ESP32)

#include "ble_esp32.h"

// Nordic UART uuids
// https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v14.0.0%2Fble_sdk_app_nus_eval.html
#define UART_SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define UART_RX_CHAR_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define UART_TX_CHAR_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define PANO_SERVICE_UUID 0x2017

// How many bytes we can send at once
#define BLOCK_SIZE 128

void Bluetooth::begin(const char *name) {
    BLEDevice::init(name);

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(&serverCallbacks);

    pServer->createService((uint16_t)PANO_SERVICE_UUID)->start();

    BLEService *pService = pServer->createService(UART_SERVICE_UUID);
    rx = pService->createCharacteristic(UART_RX_CHAR_UUID, BLECharacteristic::PROPERTY_WRITE);
    rx->setCallbacks(&rxCallbacks);

    tx = pService->createCharacteristic(UART_TX_CHAR_UUID, BLECharacteristic::PROPERTY_NOTIFY);
    tx->addDescriptor(new BLE2902()); // 0x2902: org.bluetooth.descriptor.gatt.client_characteristic_configuration

    pService->start();

    BLEAdvertising *adv = pServer->getAdvertising();
    adv->addServiceUUID((uint16_t)PANO_SERVICE_UUID);
    adv->addServiceUUID(UART_SERVICE_UUID);
    // https://www.bluetooth.com/specifications/assigned-numbers/
    adv->addServiceUUID((uint16_t)ESP_GATT_UUID_BATTERY_SERVICE_SVC);
    adv->addServiceUUID((uint16_t)ESP_GATT_UUID_DEVICE_INFO_SVC);
    adv->setAppearance((uint16_t)1856); // Generic Motorized Device
    adv->start();
}

bool Bluetooth::isConnected(void) {
    return ((ServerCallbacks *)&serverCallbacks)->isConnected(); // FIXME: temporary
}

void RXCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    std::string val = pCharacteristic->getValue();
    int len = val.length();
    if (len > 0 && abs(writeAt - readAt) < len ) {
        if (writeAt + len < buf + FIFO_SIZE) { // no wrap
            std::copy(val.begin(), val.end(), writeAt);
            writeAt += len;
        } else {
            int wrapLen = buf + FIFO_SIZE - writeAt;
            std::copy(val.begin(), val.begin() + wrapLen, writeAt);
            std::copy(val.begin() + wrapLen, val.end(), buf);
            writeAt = buf + len - wrapLen;
        }
    }
};

int RXCallbacks::read(void) {
    int c = EOF;
    if (available()) {
        c = *readAt++;
        if (readAt >= buf + FIFO_SIZE){
            readAt = buf;
        }
    }    
    return c;
}

int RXCallbacks::peek(void) {
    return available() ? (int)(*readAt) : EOF;
}

void RXCallbacks::flush(void) {
    writeAt = readAt = buf;
}

/*
 * Implement Stream interface
 */
size_t Bluetooth::write(uint8_t c) {
    if (isConnected()) {
        tx->setValue(&c, 1);
        tx->notify();
        ets_delay_us(10000); // bluetooth stack will go into congestion, if too many packets are sent
        return 1;
    }
    return 0;
};

size_t Bluetooth::write(const uint8_t *buf, size_t size) {
    size_t remain = size;
    while (isConnected() && remain > 0) {
        if (remain > BLOCK_SIZE){
            tx->setValue((uint8_t*)buf, BLOCK_SIZE);
            remain -= BLOCK_SIZE;
            buf += BLOCK_SIZE;
        } else {
            tx->setValue((uint8_t*)buf, remain);
            remain = 0;
        }
        tx->notify();
        ets_delay_us(10000);
    }        
    return size - remain;
}

int Bluetooth::available(void) {
    return rxCallbacks.available();
};
int Bluetooth::read(void) {
    return rxCallbacks.read();
};
int Bluetooth::peek(void) {
    return rxCallbacks.peek();
};
void Bluetooth::flush(void) {
    rxCallbacks.flush();
};

#endif // defined(ESP32)
