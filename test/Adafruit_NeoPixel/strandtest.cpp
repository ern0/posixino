# if ( defined(__unix__) || defined(__APPLE__) )
# define SDL_DISPLAY (0)
# include "../../src/posixino.cpp"
# else
#include <Adafruit_NeoPixel.h>
# endif


void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);
void theaterChaseRainbow(uint8_t wait);
uint32_t Wheel(byte WheelPos);


#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24 + 12, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {

	# ifdef SDL_DISPLAY
	
	if (true) {  // clock

		strip.emuSetGridScreenAnchor("se");
		strip.emuSetGridScreenPercent(30);
		strip.emuSetGridCells(36,36);
		
		int n = 0;
		
		strip.emuSetPixelPos(n++,16,0);
		strip.emuSetPixelPos(n++,20,1);
		strip.emuSetPixelPos(n++,24,2);
		strip.emuSetPixelPos(n++,28,4);
		strip.emuSetPixelPos(n++,30,8);
		strip.emuSetPixelPos(n++,31,12);
		strip.emuSetPixelPos(n++,32,16);
		strip.emuSetPixelPos(n++,31,20);
		strip.emuSetPixelPos(n++,30,24);
		strip.emuSetPixelPos(n++,28,28);
		strip.emuSetPixelPos(n++,24,30);
		strip.emuSetPixelPos(n++,20,31);
		strip.emuSetPixelPos(n++,16,32);
		strip.emuSetPixelPos(n++,12,31);
		strip.emuSetPixelPos(n++,8,30);
		strip.emuSetPixelPos(n++,4,28);
		strip.emuSetPixelPos(n++,2,24);
		strip.emuSetPixelPos(n++,1,20);
		strip.emuSetPixelPos(n++,0,16);
		strip.emuSetPixelPos(n++,1,12);
		strip.emuSetPixelPos(n++,2,8);
		strip.emuSetPixelPos(n++,4,4);
		strip.emuSetPixelPos(n++,8,2);
		strip.emuSetPixelPos(n++,12,1);
		
		strip.emuSetPixelPos(n++,16,6);
		strip.emuSetPixelPos(n++,21,7);
		strip.emuSetPixelPos(n++,25,11);
		strip.emuSetPixelPos(n++,26,16);
		strip.emuSetPixelPos(n++,25,21);
		strip.emuSetPixelPos(n++,21,25);
		strip.emuSetPixelPos(n++,16,26);
		strip.emuSetPixelPos(n++,11,25);
		strip.emuSetPixelPos(n++,7,21);
		strip.emuSetPixelPos(n++,6,16);
		strip.emuSetPixelPos(n++,7,11);
		strip.emuSetPixelPos(n++,11,7);

		for (int n = 0; n < strip.numPixels(); n++ ) {
			strip.emuSetPixelCellSize(n,4,4);
			strip.emuSetPixelPixGap(n,5,5);
		}
	
	} // if clock
	
	else {  // square
	
		strip.emuSetGridScreenAnchor("ne");
		strip.emuSetGridScreenPercent(20);
		strip.emuSetGridCells(12,3);
		for (int n = 0; n < strip.numPixels(); n++ ) {
			strip.emuSetPixelPos(n,n % 12,n / 12);
			strip.emuSetPixelCellSize(n,1,1);
			strip.emuSetPixelPixGap(n,2,2);
		} // for pixel
	
	} // else square
	
	# endif

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127,   0,   0), 50); // Red
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue

  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
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
void rainbowCycle(uint8_t wait) {
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
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

