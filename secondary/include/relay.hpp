#pragma once

#ifndef     ARDUINO_H_
#define     ARDUINO_H_
#include <Arduino.h>
#endif

class   Relay
{
    private:
        uint8_t     pin;

    public:
        void    turn_on() {
            digitalWrite(this->pin, HIGH);
            delay(50);
        };
        void    turn_off() {
            digitalWrite(this->pin, LOW);
            delay(50);
        };
        Relay(uint8_t pin) {
            this->pin = pin;
            pinMode(this->pin, OUTPUT);
        };
        ~Relay() {
            this->turn_off();
        };
};
