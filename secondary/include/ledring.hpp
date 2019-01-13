#pragma once

#ifndef     ARDUINO_H_
#define     ARDUINO_H_
#include <Arduino.h>
#endif

#include <Adafruit_NeoPixel.h>
#include "relay.hpp"

#define RELAY_LED   8

const uint32_t GAUGE_COLOR[] = {0x33cc33,  0x59cc33, 0x80c33, 0xa6cc33, 0xcccc33, 0xcca633, 0xcc8033, 0xcc5933, 0xcc3333, 0};

class   LedRing: public Relay
{
    public:
        void        begin();
        void        show();
        void        colorWipe(uint32_t, uint8_t);
        void        gauge(uint8_t);
        void        rainbow(uint8_t);
        void        rainbowCycle(uint8_t);
        void        theaterChase(uint32_t, uint8_t);
        void        theaterChaseRainbow(uint8_t);
        void        breath(uint8_t, uint8_t, uint8_t);
        uint32_t    Wheel(byte);
        uint8_t     getOperation(String);
        void        operate(uint8_t);

        LedRing(Adafruit_NeoPixel &);
        ~LedRing();

    public:
        Adafruit_NeoPixel   &strip;
};
