#include "secondary.hpp"

Secondary::Secondary() : Relay(RELAY_PIN)
{
    Serial.begin(9600);
}

Secondary::~Secondary() {};

bool        Secondary::setLEDProgram(uint8_t program)
{
    Serial.print("LED:");
    Serial.print(program);
    return true;
}
