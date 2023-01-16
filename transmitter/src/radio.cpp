#include "radio.h"

#include <Arduino.h>

#include "RF24.h"
#include "state.h"

// Inicializa o módulo de rádio
RF24 radio(7, 8);

uint8_t address[][6] = {"1Node", "2Node"};

void Radio_setupRadio() {
    // Inicializa o rádio
    // initialize the transceiver on the SPI bus
    while (1) {
        if (!radio.begin()) {
            Serial.println(F("radio hardware is not responding!!"));
            delay(500);
        } else {
            break;
        }
    }
    // Define a potência do rádio
    radio.setPALevel(RF24_PA_MAX);
    // Define a taxa de transferência do rádio
    radio.setDataRate(RF24_2MBPS);

    // Define o tamanho do payload
    radio.enableDynamicPayloads();  // ACK payloads are dynamically sized

    radio.enableAckPayload();

    // Seta o endereço de TX e RX
    radio.openWritingPipe(address[0]);
    radio.openReadingPipe(1, address[1]);

    radio.printDetails();

    radio.stopListening();
}

void Radio_checkConnection(bool connected) {
    connection.connected = connected;
    if (connected) connection.isGoodSignal = radio.testRPD();
}

void Radio_sendData(RadioCtoR* data) {
    unsigned long start_timer = micros();
    bool check = radio.write(data, sizeof(RadioCtoR));

    if (check) {
        connection.lostPackets = 0;

        if (radio.available()) {
            Radio_checkConnection(true);
            radio.read(&dataRobot, sizeof(RadioRtoC));
            unsigned long end_timer = micros();
            connection.latency = (end_timer - start_timer) / 1000;
        }
    } else {
        connection.lostPackets++;
        if (connection.lostPackets > 100) {
            Radio_checkConnection(false);
        }
    }
}