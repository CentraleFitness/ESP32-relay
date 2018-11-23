#pragma once

#ifndef     ARDUINO_H_
#define     ARDUINO_H_
#include <Arduino.h>
#endif

#include "relay.hpp"

#define RELAY_NFC   7

class   NfcBoard: public Relay
{
    public:
        NfcBoard() : Relay(RELAY_NFC) {};
        ~NfcBoard() {};
};
