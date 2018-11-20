#pragma once

#ifndef     ARDUINO_H_
#define     ARDUINO_H_
#include <Arduino.h>
#endif

#include <Adafruit_NeoPixel.h>
#include "relay.hpp"

#define RELAY_LED   8

class   LedRing: public Relay
{
    public:
        void        begin();
        void        show();
        void        colorWipe(uint32_t, uint8_t);
        void        rainbow(uint8_t);
        void        rainbowCycle(uint8_t);
        void        theaterChase(uint32_t, uint8_t);
        void        theaterChaseRainbow(uint8_t);
        void        breath(uint8_t, uint8_t, uint8_t);
        uint32_t    Wheel(byte);

        LedRing(Adafruit_NeoPixel &);
        ~LedRing();

    public:
        Adafruit_NeoPixel   &strip;
};
