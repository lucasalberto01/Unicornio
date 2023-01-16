#include "radio.h"

// Radio Communication library
#include <RF24.h>
#include <SPI.h>
#include <nRF24L01.h>

#include "state.h"

/*
 * Radio Communication settings
 */
RF24 radio(9, 10);  // CE, CSN
uint8_t address[][6] = {"1Node", "2Node"};

void Radio_setup() {
    while (1) {
        if (!radio.begin()) {
            Serial.println(F("radio hardware is not responding!!"));
            delay(500);
        } else {
            break;
        }
    }
    radio.setDataRate(RF24_2MBPS);
    radio.setPALevel(RF24_PA_MAX);
    radio.enableDynamicPayloads();  // ACK payloads are dynamically sized
    radio.enableAckPayload();
    radio.openWritingPipe(address[1]);     // 00001
    radio.openReadingPipe(1, address[0]);  // 00002

    radio.writeAckPayload(1, &ackData, sizeof(AckData));
    radio.startListening();
    radio.printDetails();
}

void Radio_proccess() {

    if (radio.available()) {
        radio.read(&comm, sizeof(comm));
        bool resp = radio.writeAckPayload(1, &ackData, sizeof(ackData));
        resp ? Serial.println("ACK sent") : Serial.println("ACK not sent");
    }
}
