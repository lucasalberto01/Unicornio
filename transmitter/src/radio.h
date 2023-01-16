#ifndef RADIO_H
#define RADIO_H
#include "RF24.h"
#include "state.h"

extern RF24 radio;
extern uint8_t address[][6];

void Radio_setupRadio();
void Radio_checkConnection(bool connected);
void Radio_sendData(RadioCtoR* data);
#endif