/*
 * Pano Controller NRF24 Radio Remote Control
 */
#ifndef RADIO_H_
#define RADIO_H_

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#undef printf

// http://tmrh20.github.io/RF24/classRF24.html

class Radio {
private:
    RF24* radio;
    byte address[1][6] = {"2PANO"};
public:
    char buffer[32];
    Radio(uint8_t ce_pin, uint8_t csn_pin);
    void begin(void);
    int available(void);
    uint8_t read(void);
    uint8_t read_type_data(uint8_t& type, void*& data);
    void write(const void *data, uint8_t len);
    void write_type_data(const uint8_t type, const void*data, uint8_t len);
};

#endif /* RADIO_H_ */
