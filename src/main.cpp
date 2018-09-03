#include <Arduino.h>


#include "PN532.h"
#include "PN532_HSU.h"

// #include "emulatetag.h"
#include "NfcAdapter.h"
#include "NdefMessage.h"

PN532_HSU   pn532hsu(Serial2);
// EmulateTag  nfc(pn532hsu);
NfcAdapter  nfc(pn532hsu);

// uint8_t     ndefBuf[120];
// NdefMessage message;
// int         messageSize;

// uint8_t     uid[3] = {0x12, 0x34, 0x56};

NdefMessage message = NdefMessage();

void setup() {
    // nfc.begin();
    Serial.begin(115200);
    Serial.print("--- Centrale Fitness Beta Module [Test] ---");
    message.addTextRecord("Centrale Fitness");
    nfc.begin();
}

void loop() {
    if (nfc.tagPresent()) {
        if (nfc.write(message)) {
            Serial.println("Write success!");
        }
        else {
            Serial.println("Write failed :(");
        }
    }
    delay(5000);
}
