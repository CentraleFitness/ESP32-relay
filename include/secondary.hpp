#pragma once
#include "relay.hpp"

#define     RELAY_PIN   19

class   Secondary: public Relay
{
    public:
        bool    setLEDProgram(uint8_t);
        bool    setNFCId(String);
        Secondary();
        ~Secondary();
};
