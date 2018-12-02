#include "secondary.hpp"

Secondary::Secondary(HardwareSerial *HSerial) : Relay(RELAY_PIN)
{
    serial = HSerial;
    serial->begin(9600, SERIAL_8N1, 16, 17);
    serial->flush();
    serial->setTimeout(5000);
}

Secondary::~Secondary() {};

bool        Secondary::isOn(void)
{
    String  buff;

    buff = serial->readStringUntil(':');
    Serial.print("cmd from secondary: ");
    Serial.println(buff);
    if (buff.endsWith("ON"))
        return (true);
    if (!buff.endsWith("INFO"))
        return (false);
    buff = serial->readString();
    Serial.print("val from secondary: ");
    Serial.println(buff);
    if (!buff.startsWith("ON"))
        return (false);
    return (true);
}

bool        Secondary::setLEDProgram(uint8_t program)
{
    String  ack;

    serial->print("LED:");
    serial->print(program);
    ack = serial->readString();
    if (ack != "ACK:OK")
        return (false);
    return (true);
}

bool        Secondary::setNFCId(String id)
{
    String  ack;

    serial->print("NFC:");
    serial->print(id);
    ack = serial->readString();
    if (ack != "ACK:OK")
        return (false);
    return (true);
}
