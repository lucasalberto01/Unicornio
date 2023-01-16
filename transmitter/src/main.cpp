#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SPI.h>

#include "printf.h"
#include "radio.h"
#include "state.h"

#define L1X A0
#define L1Y A1
#define L1B A2

#define L2X A4
#define L2Y A5
#define L2B A6

// Inicializa o LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void SetupSerial() {
    // Inicializa a serial
    Serial.begin(9600);
    // Inicializa o printf
    printf_begin();
}

void SetupDisplay() {
    // Define o número de colunas e linhas do LCD
    lcd.begin(20, 4);
}

void SetupPin() {
    pinMode(L1B, INPUT_PULLUP);
    pinMode(L2B, INPUT_PULLUP);
}

void setup() {
    // Inicializa a serial
    SetupSerial();
    // Inicializa o LCD
    SetupDisplay();
    // Inicializa os pinos
    SetupPin();
    // Inicializa o rádio
    Radio_setupRadio();
}

void DisplayRadio() {
    if (connection.connected) {
        lcd.setCursor(0, 0);
        lcd.print("Conectado");
        lcd.setCursor(20 - 6, 0);
        lcd.print(connection.isGoodSignal ? "+64dBm" : "-64dBm");

    } else {
        lcd.setCursor(0, 0);
        lcd.print("Desconectado");
    }

    lcd.setCursor(0, 1);
    if (connection.lostPackets < 999) {
        lcd.print("Lost: ");
        lcd.print(connection.lostPackets);
    } else {
        lcd.print("Lost:+999");
        connection.lostPackets = 1000;
    }

    lcd.setCursor(10, 1);
    lcd.print("Ping: ");
    lcd.print(connection.latency);
}

void DisplayControl(int* valX, int* valY, bool* valBtn) {
    char line = 3;
    lcd.setCursor(0, line);
    lcd.print("X1: ");
    lcd.print(*valX);

    lcd.setCursor(9, line);
    lcd.print("Y1: ");
    lcd.print(*valY);

    lcd.setCursor(19, line);
    (*valBtn) ? lcd.print("#") : lcd.print(" ");
}

void DisplayStrucSend(RadioCtoR* data) {
    char line = 2;
    lcd.setCursor(0, line);
    lcd.print("MR: ");
    lcd.print(data->velA);

    lcd.setCursor(9, line);
    lcd.print("ML: ");
    lcd.print(data->velB);
}

void loop() {
    // Limpa a tela
    lcd.clear();

    int valX = analogRead(L1X);
    int valY = analogRead(L2Y);
    bool valBtn = digitalRead(L2B) == LOW;

    RadioCtoR data;

    // Display data in LCD
    DisplayRadio();
    DisplayControl(&valX, &valY, &valBtn);

    int limitVel = 255;
    int maxVel = 0;

    if (valX < 500) {
        maxVel = -map(valX, 512, 0, 0, limitVel);
    } else if (valX > 550) {
        maxVel = map(valX, 512, 1023, 0, limitVel);
    } else {
        maxVel = 0;
    }

    if (valY < 500 and maxVel != 0) {
        data.velB = maxVel;
        data.velA = map(valY, 512, 0, maxVel, 0);
    } else if (valY > 550 and maxVel != 0) {
        data.velB = map(valY, 512, 1023, maxVel, 0);
        data.velA = maxVel;
    } else if (valY < 500 and maxVel == 0) {
        data.velA = -map(valY, 512, 0, 0, limitVel);
        data.velB = map(valY, 512, 0, 0, limitVel);
    } else if (valY > 550 and maxVel == 0) {
        data.velA = map(valY, 512, 1023, 0, limitVel);
        data.velB = -map(valY, 512, 1023, 0, limitVel);
    } else {
        data.velA = maxVel;
        data.velB = maxVel;
    }

    DisplayStrucSend(&data);
    Radio_sendData(&data);
}
