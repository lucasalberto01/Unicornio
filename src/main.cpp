#include <Arduino.h>

// Thread library
#include <Thread.h>
#include <ThreadController.h>

// Extra libraries
#include "move/move.h"
#include "printf.h"
#include "radio/radio.h"
#include "state.h"

Communication comm = (Communication){0, 0};
AckData ackData = (AckData){0, 0};

// Master Thread
ThreadController cpuMain = ThreadController();

// My Thread
Thread moveThread = Thread();

// His Thread
Thread commThread = Thread();

// Blink Led Thread
Thread statusThread = Thread();

void ThrStatus() {
}

// ########### SETUP ########### //

void ThreadSetup() {
    moveThread.onRun(Move_process);
    moveThread.setInterval(10);

    commThread.onRun(Radio_proccess);
    commThread.setInterval(10);

    statusThread.onRun(ThrStatus);
    statusThread.setInterval(1000);

    cpuMain.add(&moveThread);
    cpuMain.add(&commThread);
    cpuMain.add(&statusThread);
}

void SerialSetup() {
    Serial.begin(9600);
    printf_begin();
}

void setup() {
    SerialSetup();
    Move_setup();
    Radio_setup();
    ThreadSetup();
}

// ########### LOOP ########### //
void loop() {
    cpuMain.run();
    Move_process_PID();
}