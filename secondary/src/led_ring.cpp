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
    this->strip.setBrightness(64);
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

void  LedRing::gauge(uint8_t pos)
{
  for (uint8_t i = 0; i < strip.numPixels() && i < pos; ++i)
  {
    strip.setPixelColor(
      i, GAUGE_COLOR[(uint32_t)((float)i / (float)strip.numPixels() * 9)]);
    strip.show();
  }
  for (uint8_t i = pos; i < strip.numPixels(); ++i)
  {
    strip.setPixelColor(i , 0);
    strip.show();
  }
}

void LedRing::rainbow(uint8_t wait)
{
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

  for(j=0; j<256*1; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void LedRing::theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<1; j++) {  //do 1 cycle of chasing
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
void LedRing::theaterChaseRainbow(uint8_t wait)
{
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

void LedRing::breath(uint8_t r, uint8_t g, uint8_t b)
{
    for (uint8_t i = 0; i < 255 ; ++i) {
        for(uint16_t j = 0; j < strip.numPixels(); ++j) {
            strip.setPixelColor(j, strip.Color(r *  i / 255, g * i / 255, b * i / 255));
        }
        strip.show();
        delay(5);
    }
    for (uint8_t i = 225; i > 0; --i) {
        for(uint16_t j = 0; j < strip.numPixels(); ++j) {
            strip.setPixelColor(j, strip.Color(r *  i / 255, g * i / 255, b * i / 255));    }
        strip.show();
        delay(5);
    }
    this->colorWipe(0, 0);
    this->strip.show();
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t LedRing::Wheel(byte WheelPos)
{
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

uint8_t   LedRing::getOperation(String cmd)
{
  long    prog_num = 0;

  if (cmd == "off") {
    this->colorWipe(0, 0);
    this->turn_off();
    return (0);
  }
  else {
    prog_num = cmd.toInt();
  }
  return (uint8_t)prog_num;
}

void      LedRing::operate(uint8_t prog_num)
{
  if (prog_num >= 0 && prog_num <= 9 && !this->status())
    this->turn_on();
  switch (prog_num) {
    case 1:
      this->colorWipe(0xFFFFFF, 1);
      break;
    case 2:
      this->colorWipe(0x00FF00, 1);
      break;
    case 3:
      this->colorWipe(0xFF0000, 1);
      break;
    case 4:
      this->rainbow(10);
      break;
    case 5:
      this->rainbowCycle(10);
      break;
    case 6:
      this->theaterChaseRainbow(10);
      break;
    case 7:
      this->breath(0xFF, 0xFF, 0xFF);
      break;
    case 8:
      this->breath(0xFF, 0, 0);
      break;
    case 9:
      this->breath(0, 0xFF, 0);
      break;
    default:
      if (prog_num > 9)
      {
        this->gauge(prog_num - 10);
      }
      break;
  }
}