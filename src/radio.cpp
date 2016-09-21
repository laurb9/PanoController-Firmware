/*
 * Pano Controller NRF24 Radio Remote Control
 */

#include "radio.h"

Radio::Radio(uint8_t ce_pin, uint8_t csn_pin){
    radio = new RF24(ce_pin, csn_pin);
    connected = false;
    last_received_ts = millis();
}

void Radio::begin(void){
    radio->begin();
    radio->setChannel(76); // default is 76
    radio->setPALevel(RF24_PA_LOW); // RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
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
    radio->printDetails();
}

static void debug(const char *msg, const void *buf, int len){
    return;
    Serial.print(msg);
    Serial.print(len);
    Serial.print(" -> ");
    while (len--){
        Serial.print(*(const char*)buf, HEX);
        buf += 1;
        Serial.print(" ");
    }
    Serial.println();
}

int Radio::available(void){
    int count = radio->available();
    if (count > 0){
        last_received_ts = millis();
        if (!connected){
            Serial.println("NRF24 Connected");
            connected = true;
        }
    } else if (connected && millis() - last_received_ts > 10000){
        Serial.println("NRF24 Connection Lost");
        connected = false;
    }
    return count;
}

uint8_t Radio::read(void){
    uint8_t len = 0;
    if (available()){
        len = radio->getDynamicPayloadSize();
        radio->read(buffer, len);
        debug("Radio Received ", buffer, len);
    }
    return len;
}

void Radio::write(const void *data, uint8_t len){
    debug("Radio Send ", data, len);
    radio->stopListening();
    if (!radio->write(data, len)){
        connected = false;
    }
    radio->startListening();
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
    len--;
    data = (len) ? buffer + 1 : NULL;
    return len;
}

/*
 * Helper method to send out a packet type with the data buffer
 */
void Radio::write_type_data(const uint8_t type, const void* data, uint8_t len,
                            const bool delayed){
    if (len == 0 && data){
        len = strlen((const char*)data);
    }
    len = min(len, 32);
    buffer[0] = type;
    for (int i=0; i < len && i < 31; i++){
        buffer[i+1] = ((const char*)data)[i];
    }
    if (!delayed){
        write(buffer, len+1);
    } else {
        radio->writeAckPayload(1, buffer, len+1);
    }
}
