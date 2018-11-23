#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <emulatetag.h>
#include <NdefMessage.h>
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

NfcBoard        nfcBoard;

LedRing        led(strip);
PN532_SPI      pn532(SPI, PN532_SS);
EmulateTag      nfc(pn532);

uint8_t       uid[3] = {0x12, 0x34, 0x56};
uint8_t       ndefBuf[120];
NdefMessage   message = NdefMessage();
int           messageSize = 0;

void  emulate_session_id()
{
  nfc.emulate(5000);
}

void setup() {
  Serial.begin(9600);
  Serial.print("Booting...");

  nfcBoard.turn_on();
  Serial.println("NFC ON");
  memset(ndefBuf, 0, 120);
  message.addUriRecord("https://github.com/CentraleFitness");
  messageSize = message.getEncodedSize();
  message.encode(ndefBuf);
  nfc.setNdefFile(ndefBuf, messageSize);
  nfc.setUid(uid);
  nfc.init();
  delay(100);

  Serial.println("NFC initialized");

  led.turn_on();
  Serial.println("LED ON");
  led.begin();
  led.show(); // Initialize all pixels to 'off'
  Serial.println("Done!");
}

void loop()
{
//   Serial.println("Program 1");
  led.breath(0xff, 0xff, 0xff);

  emulate_session_id();

//   led.colorWipe(strip.Color(255, 0, 0), 50); // Red
//   led.colorWipe(strip.Color(0, 255, 0), 50); // Green
//   led.colorWipe(strip.Color(0, 0, 255), 50); // Blue
// // //colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
// //   // Send a theater pixel chase in...
// //   Serial.println("Program 3");
//   led.theaterChase(strip.Color(127, 127, 127), 50); // White
// //   Serial.println("Program 4");
//   led.theaterChase(strip.Color(127, 0, 0), 50); // Red
// //   Serial.println("Program 5");
//   led.theaterChase(strip.Color(0, 0, 127), 50); // Blue

// //   Serial.println("Program 6");
// //   led.rainbow(20);
// //   Serial.println("Program 7");
// //   led.rainbowCycle(20);
// //   Serial.println("Program 8");
//   led.theaterChaseRainbow(50);

//   led.rainbowCycle(20);
}
