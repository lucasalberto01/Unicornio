#include <Arduino.h>

// Thread library
#include <Thread.h>
#include <ThreadController.h>

// Radio Communication library
#include <RF24.h>
#include <SPI.h>
#include <nRF24L01.h>

// Movement library
#include <L298NX2.h>

// Extra libraries
#include "printf.h"

/*
 * Radio Communication settings
 */
RF24 radio(9, 10);  // CE, CSN
uint8_t address[][6] = {"1Node", "2Node"};

/*
 * Movement settings
 */
const unsigned int EN_A = 3;  // PWM
const unsigned int IN1_A = 2;
const unsigned int IN2_A = 4;
const unsigned int EN_B = 6;   // PWM
const unsigned int IN1_B = 5;  // PWM not needed
const unsigned int IN2_B = 7;
L298NX2 motors(EN_A, IN1_A, IN2_A, EN_B, IN1_B, IN2_B);

/*
 * Thread settings
 */
//
ThreadController cpuMain = ThreadController();

// My Thread
Thread moveThread = Thread();

// His Thread
Thread commThread = Thread();

// Blink Led Thread
Thread statusThread = Thread();

// ############################ //

struct Communication_struct {
    int velA;
    int velB;
};
typedef struct Communication_struct Communication;

struct AckData_struct {
    int unsigned odom1;
    int unsigned odom2;
};
typedef struct AckData_struct AckData;

Communication comm = (Communication){0, 0};
AckData ackData = (AckData){0, 0};

// Blink Led
void ThrStatus() {
}

// Receive radio communication
void ThrComm() {
    uint8_t pipe;

    if (radio.available(&pipe)) {
        radio.read(&comm, sizeof(comm));
        bool resp = radio.writeAckPayload(1, &ackData, sizeof(ackData));
        resp ? Serial.println("ACK sent") : Serial.println("ACK not sent");
        ackData.odom1++;
        ackData.odom2++;
    }
}

void ThrMove() {
    // Serial.print("Received: ");
    // Serial.print(comm.velA);
    // Serial.print(" ");
    // Serial.println(comm.velB);

    if (comm.velA > 0) {
        motors.setSpeedA(comm.velA);
        motors.forwardA();

    } else if (comm.velA < 0) {
        motors.setSpeedA(-comm.velA);
        motors.backwardA();

    } else {
        motors.setSpeedA(0);
        motors.stopA();
    }

    if (comm.velB > 0) {
        motors.setSpeedB(comm.velB);
        motors.forwardB();

    } else if (comm.velB < 0) {
        motors.setSpeedB(-comm.velB);
        motors.backwardB();

    } else {
        motors.setSpeedB(0);
        motors.stopB();
    }
}

// ########### SETUP ########### //

void ThreadSetup() {
    moveThread.onRun(ThrMove);
    moveThread.setInterval(100);

    commThread.onRun(ThrComm);
    commThread.setInterval(100);

    statusThread.onRun(ThrStatus);
    statusThread.setInterval(1000);

    cpuMain.add(&moveThread);
    cpuMain.add(&commThread);
    cpuMain.add(&statusThread);
}

void PinSetup() {
}

void SerialSetup() {
    Serial.begin(9600);
    printf_begin();
}

void CommunicationSetup() {
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

void setup() {
    PinSetup();
    SerialSetup();
    CommunicationSetup();
    ThreadSetup();
}

// ########### LOOP ########### //
void loop() {
    cpuMain.run();
}