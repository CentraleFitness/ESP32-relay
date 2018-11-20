#include "ledring.hpp"


void    LedRing::begin()
{
    this->strip.begin();
}

void    LedRing::show()
{
    this->strip.show();
}

LedRing::LedRing(Adafruit_NeoPixel &rgb_strip): Relay(RELAY_LED), strip(rgb_strip)
{
    pinMode(RELAY_LED, OUTPUT);
};

LedRing::~LedRing()
{
    this->turn_off();
}


// Fill the dots one after the other with a color
void LedRing::colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void LedRing::rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void LedRing::rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void LedRing::theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void LedRing::theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}



// void LedRing::breath() {
//   for (int i = 0; i < 225 ; i++) {
//     for(int j=0; j<strip.numPixels(); j++) {
//       strip.setPixelColor(j, strip.Color(i+15,i,0)); // yellow with a little extra red to make it warmer 
//     }
//     strip.show();
//     delay(10);
//   }
//   for (int i = 225; i > 0; i--) {
//     for(int j=0; j<strip.numPixels(); j++) {
//       strip.setPixelColor(j, strip.Color(i+15,i,0));
//     }
//     strip.show();
//     delay(10);
//   }
// }

void LedRing::breath(uint8_t r, uint8_t g, uint8_t b)
{
    for (uint8_t i = 0; i < 255 ; ++i) {
        for(uint16_t j = 0; j < strip.numPixels(); ++j) {
            strip.setPixelColor(j, strip.Color(r *  i / 255, g * i / 255, b * i / 255));
        }
        strip.show();
        delay(10);
    }
    for (uint8_t i = 225; i > 0; --i) {
        for(uint16_t j = 0; j < strip.numPixels(); ++j) {
            strip.setPixelColor(j, strip.Color(r *  i / 255, g * i / 255, b * i / 255));    }
        strip.show();
        delay(10);
    }
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t LedRing::Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
