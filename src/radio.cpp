/*
 * Pano Controller NRF24 Radio Remote Control
 */

#include "radio.h"

Radio::Radio(uint8_t ce_pin, uint8_t csn_pin){
    radio = new RF24(ce_pin, csn_pin);
}

void Radio::begin(void){
    radio->begin();
    radio->setChannel(76); // default is 76
    radio->setPALevel(RF24_PA_MIN); // RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
    radio->setCRCLength(RF24_CRC_16);
    radio->setDataRate(RF24_1MBPS); // RF24_250KBPS, RF24_1MBPS, RF24_2MBPS
    // retry timeout window needs to allow for ack payload, see spec pdf
    radio->setRetries(4, 10); // 4*250us, 10 retries
    radio->setAutoAck(true); // default true
    //radio->maskIRQ(1, 1, 1); // tx_ok=1, tx_fail=1, rx_ready=1
    radio->enableDynamicPayloads();
    radio->openWritingPipe(address[0]);
    radio->openReadingPipe(1, address[0]);
    radio->powerUp();
    radio->startListening();
    radio->enableAckPayload();
    radio->writeAckPayload(1, "ACK", 3); // for next packet received
    radio->printDetails();
}

static void SerialPrintHex(const void *buf, int len){
    while (len--){
        Serial.print(*(const char*)buf, HEX);
        buf += 1;
        Serial.print(" ");
    }
    Serial.println();
}

int Radio::available(void){
    return radio->available();
}

uint8_t Radio::read(void){
    uint8_t len = 0;
    if (radio->available()){
        len = radio->getDynamicPayloadSize();
        radio->read(buffer, len);
        Serial.print("Radio Received ");
        Serial.print(len);
        Serial.print(" -> ");
        SerialPrintHex(buffer, len);
        radio->writeAckPayload(1, "ACK", 3); // for FUTURE packets received
    }
    return len;
}

void Radio::write(const void *data, uint8_t len){
    Serial.print("Radio Send ");
    Serial.print(len);
    Serial.print(" -> ");
    SerialPrintHex(data, len);
    radio->stopListening();
    radio->write(data, len);
    radio->startListening();
    if (radio->available()){
        len = radio->getDynamicPayloadSize();
        radio->read(buffer, len);
        if (len != 3 || strncmp(buffer, "ACK", 3)){
            Serial.print("Unknown ack ");
            SerialPrintHex(buffer, len);
        }
    }
}

/*
 * Helper method to receive a packet type with the data buffer
 */
uint8_t Radio::read_type_data(uint8_t& type, void*& data){
    uint8_t len = read();
    if (len <= 0){
        return 0;
    }
    type = *(uint8_t*)buffer;
    data = buffer + 1;
    return len;
}

/*
 * Helper method to send out a packet type with the data buffer
 */
void Radio::write_type_data(const uint8_t type, const void*data, uint8_t len){
    len = min(len, 32);
    buffer[0] = type;
    for (int i=0; i < len && i < 31; i++){
        buffer[i+1] = ((const char*)data)[i];
    }
    write(buffer, len+1);
}
