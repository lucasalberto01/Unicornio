#include "move.h"

// Movement library
#include <L298NX2.h>
#include <PID_v1.h>

#include "state.h"

/*
 * Movement settings
 */
const unsigned int EN_A = 5;    // PWM
const unsigned int IN1_A = 14;  // A0
const unsigned int IN2_A = 15;  // A1

const unsigned int EN_B = 6;    // PWM
const unsigned int IN1_B = 16;  // A2
const unsigned int IN2_B = 17;  // A3
L298NX2 motors(EN_A, IN1_A, IN2_A, EN_B, IN1_B, IN2_B);

/*
 * PID settings
 */
unsigned long TIME;
unsigned int pulse_per_turn = 10;

const unsigned int ENCODER_A = 2;  // interrupt pin
const unsigned int ENCODER_B = 3;  // interrupt pin

double rpmA;
double rpmB;

volatile byte pulsesA;
volatile byte pulsesB;

double driverOutA = 0;
double driverOutB = 0;

double setPointA = 0;
double setPointB = 0;

float Kp = 0.6;
float Ki = 5;
float Kd = 0;

PID pidA(&rpmA, &driverOutA, &setPointA, Kp, Ki, Kd, DIRECT);
PID pidB(&rpmB, &driverOutB, &setPointB, Kp, Ki, Kd, DIRECT);

void Move_interruptA() {
    pulsesA++;
}

void Move_interruptB() {
    pulsesB++;
}

void Move_setup() {
    pidA.SetMode(AUTOMATIC);
    pidA.SetSampleTime(100);
    pidB.SetMode(AUTOMATIC);
    pidB.SetSampleTime(100);

    pinMode(ENCODER_A, INPUT);
    pinMode(ENCODER_B, INPUT);

    attachInterrupt(digitalPinToInterrupt(ENCODER_A), Move_interruptA, FALLING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_B), Move_interruptB, FALLING);

    pinMode(EN_A, OUTPUT);
    pinMode(IN1_A, OUTPUT);
    pinMode(IN2_A, OUTPUT);

    pinMode(EN_B, OUTPUT);
    pinMode(IN1_B, OUTPUT);
    pinMode(IN2_B, OUTPUT);
}

void Move_process_PID() {
    if (millis() - TIME >= 100) {                           // updating every 0.1 second
        detachInterrupt(digitalPinToInterrupt(ENCODER_A));  // turn off trigger
        detachInterrupt(digitalPinToInterrupt(ENCODER_B));  // turn off trigger
        // calcuate for rpm
        rpmA = (60 * 100 / pulse_per_turn) / (millis() - TIME) * pulsesA;
        rpmB = (60 * 100 / pulse_per_turn) / (millis() - TIME) * pulsesB;
        TIME = millis();
        pulsesA = 0;
        pulsesB = 0;
        // trigger count function everytime the ENCODER turns from HIGH to LOW
        attachInterrupt(digitalPinToInterrupt(ENCODER_A), Move_interruptA, FALLING);
        attachInterrupt(digitalPinToInterrupt(ENCODER_B), Move_interruptB, FALLING);
    }
    pidA.Compute();
    pidB.Compute();

    Serial.print("rpmA: ");
    Serial.print(rpmA);
    Serial.print("setPointA: ");
    Serial.print(setPointA);
    Serial.print(" outPutA: ");
    Serial.println(driverOutA);

    motors.setSpeedA(driverOutA);
    motors.setSpeedB(driverOutB);
}

void Move_process_PID_test() {
    if (comm.velA > 0) {  // forward
        setPointA = comm.velA;
        motors.forwardA();

    } else if (comm.velA < 0) {  // backward
        setPointA = -comm.velA;
        motors.backwardA();
    } else {  // stop
        setPointA = 0;
        motors.stopA();
    }

    if (comm.velB > 0) {  // forward
        setPointB = comm.velB;
        motors.forwardB();

    } else if (comm.velB < 0) {  // backward
        setPointB = -comm.velB;
        motors.backwardB();
    } else {  // stop
        setPointB = 0;
        motors.stopB();
    }
    // Serial.print("velA: ");
    // Serial.print(setPointA);
    // Serial.print(" velB: ");
    // Serial.println(setPointB);
}

void Move_process() {
    if (comm.velA > 0) {  // forward
        motors.forwardA();
        motors.setSpeedA(comm.velA);

    } else if (comm.velA < 0) {  // backward
        motors.backwardA();
        motors.setSpeedA(-comm.velA);
    } else {  // stop
        motors.stopA();
    }

    if (comm.velB > 0) {  // forward
        motors.forwardB();
        motors.setSpeedB(comm.velB);

    } else if (comm.velB < 0) {  // backward
        motors.backwardB();
        motors.setSpeedB(-comm.velB);
    } else {  // stop
        motors.stopB();
    }
}