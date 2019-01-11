#include "secondary.hpp"

Secondary::Secondary(HardwareSerial *HSerial) : Relay(RELAY_PIN)
{
    serial = HSerial;
    serial->begin(9600, SERIAL_8N1, 16, 17);
    serial->flush();
    serial->setTimeout(500);
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

bool            Secondary::receiveAck(uint8_t retries = 5, uint16_t timeout = 500)
{
    String      ack;

    ack = serial->readString();
    while (ack.indexOf("ACK:OK") < 0)
    {
        // Serial.println("retrying...");
        retries -= 1;
        if (retries == 0)
        {
            Serial.println("ack not received");
            return (false);
        }
        delay(500);
        ack = serial->readString();
    }
    return (true);
}

bool            Secondary::setLEDProgram(uint8_t program)
{

    String      ack;

    serial->print("LED:");
    serial->print(program);
    // Serial.print("info sent");
    delay(100);
    return receiveAck();
}

bool        Secondary::setNFCId(String id)
{
    String  ack;

    // Serial.println("writting nfc id...");
    serial->print("NFC:");
    serial->print(id);
    return receiveAck();
}

bool        Secondary::buttonPressed(void)
{
    String  msg;

    msg = serial->readString();
    // Serial.println(msg);
    // Serial.println(msg.indexOf("BUTTON:P"));
    if (msg.indexOf("BUTTON:P") != -1)
    {
        Serial.print("Button pressed!");
        return (true);
    }
    return (false);
}