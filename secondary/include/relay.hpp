#pragma once

#ifndef     ARDUINO_H_
#define     ARDUINO_H_
#include <Arduino.h>
#endif

class   Relay
{
    private:
        uint8_t     mPin;
        bool        mStatus;

    public:
        void    turn_on(void) {
            digitalWrite(this->mPin, HIGH);
            this->mStatus = true;
            delay(10);
        };
        void    turn_off(void) {
            digitalWrite(this->mPin, LOW);
            this->mStatus = false;
            delay(10);
        };
        bool    status(void) {
            return (this->mStatus);
        }
        Relay(uint8_t pin) {
            this->mStatus = false;
            this->mPin = pin;
            pinMode(this->mPin, OUTPUT);
        };
        ~Relay() {
            this->turn_off();
        };
};
