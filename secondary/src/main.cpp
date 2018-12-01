#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <emulatetag.h>
#include <NdefMessage.h>
#include <NfcAdapter.h>
#include "ledring.hpp"
#include "nfcboard.hpp"

#define PN532_SS    10
#define LED_DIN     9

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel   strip = Adafruit_NeoPixel(24, LED_DIN, NEO_GRB + NEO_KHZ800);
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


LedRing         led(strip);
uint8_t         led_prog = 0;

PN532_SPI       pn532(SPI, PN532_SS);

uint8_t         uid[3] = {0x12, 0x34, 0x56};
uint8_t         ndefBuf[120];
NdefMessage     message = NdefMessage();
int             messageSize = 0;

bool            write_session_id(String session_id)
{
  NfcBoard      nfcBoard;
  NfcAdapter    nfc(pn532);
  NdefMessage   message;

  nfcBoard.turn_on();
  nfc.begin();
  message.addTextRecord(session_id);
  if (nfc.tagPresent(5000)) {
    for (uint8_t i = 0; !nfc.write(message); i += 1) {
      if (i == 5) {
        Serial.println("Write failed after 5 attemtps");
        return false;
      }
    }
  }
  return true;
}

void              emulate_session_id()
{
  NfcBoard        nfcBoard;
  EmulateTag      nfc(pn532);

  nfcBoard.turn_on();
  Serial.println("NFC ON");
  memset(ndefBuf, 0, 120);
  message.addUriRecord("https://github.com/CentraleFitness");
  messageSize = message.getEncodedSize();
  message.encode(ndefBuf);
  nfc.setNdefFile(ndefBuf, messageSize);
  nfc.setUid(uid);
  nfc.init();
  Serial.println("Emualating for 5000ms...");
  if (nfc.emulate(5000)) {
    Serial.println("Write occured");
  }
  else {
    Serial.println("Timeout");
  }
}

void      setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial1.println("INFO:ON");
  led.turn_on();
  led.begin();
  led.show();
  led.rainbow(10);
  led.colorWipe(0xFFFFFF, 0);
}


void      loop()
{
  String  incoming_type;
  String  incoming_cmd;

  if (Serial1.available() > 0) {
    incoming_type = Serial1.readStringUntil(':');
    Serial1.println(incoming_type);
    if (incoming_type == "LED") {
      incoming_cmd = Serial1.readString();
      led_prog = led.getOperation(incoming_cmd);
      Serial1.println("ACK:OK");
    } else if (incoming_type == "NFC") {
      incoming_cmd = Serial1.readString();
      write_session_id(incoming_cmd);
      Serial1.println("ACK:OK");
    }
  }
  if (led_prog != 0) {
    led.operate(led_prog);
  }
  delay(10);
}
