#pragma once
#include "relay.hpp"

#ifndef ARDUINO_H_
#define ARDUINO_H_
#include <Arduino.h>
#endif

#define     RELAY_PIN   19
#define     RX_PIN      16
#define     TX_PIN      17

#ifndef     H_SERIAL_
#define     H_SERIAL_
#endif

class   Secondary: public Relay
{
    private:
        HardwareSerial  *serial;

    public:
        bool    receiveAck(uint8_t, uint16_t);
        bool    setLEDProgram(uint8_t);
        bool    setNFCId(String);
        bool    isOn(void);
        bool    buttonPressed(void);
        Secondary(HardwareSerial *);
        ~Secondary();
};
