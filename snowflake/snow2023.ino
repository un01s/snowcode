/*
 *  snowflake 2023 warmbit
 *  
 *  the base sketch is on the github, snow_twinkle03.ino
 *  
 *  
 */
/*
 * File: fadein
 * 
 * By: Andrew Tuline
 * 
 * Date: April, 2019
 * 
 * Based previous work (namely twinklefox) by Mark Kriegsman, this program shows how you can fade-in twinkles by using the fact that a random number generator
 * with the same seed will generate the same numbers every time. Combine that with millis and a sine wave and you have twinkles fading in/out.
 * 
 * Consider this a poor man's version of twinklefox.
 * 
 * Source: https://gist.github.com/atuline
 * 
 * the palette, the changing od color is smooth fade in and out.
 * 
 * THIS is GOOD.
 * 
 */

#include "FastLED.h"                                          // FastLED library.

// Fixed definitions cannot change on the fly.
#define LED_PIN     A1                                             // Data pin to connect to the strip.
#define COLOR_ORDER GRB                                       // It's GRB for WS2812 and BGR for APA102.
#define LED_TYPE    WS2812B                                       // Using APA102, WS2812, WS2801. Don't forget to modify LEDS.addLeds to suit.
#define NUM_LEDS    50   // Number of LED's.

#define BRIGHTNESS  64
#define MIN_BRIGHTNESS  8
#define MAX_BRIGHTNESS  64 

// Global variables can be changed on the fly.
uint8_t max_bright = 128;                                     // Overall brightness.

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.

// Palette definitions
CRGBPalette16 currentPalette = PartyColors_p;
CRGBPalette16 targetPalette;
TBlendType    currentBlending = LINEARBLEND;                  // NOBLEND or LINEARBLEND

#define UPDATES_PER_SECOND 10

const uint8_t rings[5][6] = {
{ 0, 0, 0, 0, 0, 0},
{ 1, 2, 3, 4, 5, 6},
{ 9,12,15,18,21,24},
{ 8,11,14,17,20,23},
{ 7,10,13,16,19,22},
};

// 0 is always at the center
const uint8_t shortBranches[6][3] = {
{ 9,  8,  7},
{12, 11, 10},
{15, 14, 13},
{18, 17, 16},
{21, 20, 19},
{24, 23, 22},
};

// both short and long branches does not have all LEDs on the board
const uint8_t longBranches[3][7] = {
{ 7,  8,  9, 0, 18, 17, 16},
{10, 11, 12, 0, 21, 20, 19},
{13, 14, 15, 0, 24, 23, 22}, 
};

void setup() {

  LEDS.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
 
  FastLED.setBrightness(max_bright);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);               // FastLED Power management set at 5V, 500mA.
  
} // setup()


void loop() {

  ChangePalettePeriodically();
  uint8_t maxChanges = 24; 
  nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
  
  static uint8_t startIndex = 0;
  //fadein();
  FillRingsFromPaletteColors(startIndex);

  startIndex = startIndex + 2; /* motion speed */
  
  FastLED.show();
  FastLED.delay(1000/UPDATES_PER_SECOND);
  
} // loop()

// the direction is from the center to outlines
void FillRingsFromPaletteColors( uint8_t colorIndex)
{
    // LINEARBLEND is better than NOBLEND, it makes the animation smoother.
    uint8_t i, j, index;
    CRGB c1 = ColorFromPalette( currentPalette, colorIndex, MAX_BRIGHTNESS, LINEARBLEND);
    CRGB c2 = ColorFromPalette( currentPalette, (colorIndex+4)%256, MAX_BRIGHTNESS, LINEARBLEND);
    CRGB c3 = ColorFromPalette( currentPalette, (colorIndex+8)%256, MAX_BRIGHTNESS, LINEARBLEND);
    CRGB c4 = ColorFromPalette( currentPalette, (colorIndex+16)%256, MAX_BRIGHTNESS, LINEARBLEND);
    CRGB c5 = ColorFromPalette( currentPalette, (colorIndex+20)%256, MAX_BRIGHTNESS, LINEARBLEND);

    for (i = 0; i < 5; i++) {
      for (j = 0; j < 6; j++) {
        index = rings[i][j];
        switch(i) {
          case 0:
            leds[index] = c1;
            leds[index+25] = c1;
            break;
          case 1: 
            leds[index] = c2;
            leds[index+25] = c2;
            break;
          case 2:
            leds[index] = c3;
            leds[index+25] = c3;
            break;
          case 3: 
            leds[index] = c4;
            leds[index+25] = c4;
            break;
          case 4:
            leds[index] = c5;
            leds[index+25] = c5;
            break;
          default:
            leds[index] = CRGB::Gray;
            break;
        }
      }
      colorIndex += 2; // 4, small steps better?
    }
}

void ChangePalettePeriodically()
{
  uint8_t secondHand = (millis() / 1000) % 120;
  static uint8_t lastSecond = 99;
  
  if( lastSecond != secondHand) {
    lastSecond = secondHand;
    CRGB r = CHSV( HUE_RED, 255, 255);
    CRGB p = CHSV( HUE_PURPLE, 255, 255);
    CRGB y = CHSV( HUE_YELLOW, 255, 255);
    CRGB o = CHSV( HUE_ORANGE, 255, 255);
    CRGB g = CHSV( HUE_GREEN, 255, 255);
    CRGB b = CRGB::Blue;
    CRGB black = CRGB::Black;
    CRGB w = CRGB::White;
    CRGB gray = CRGB::Gray;
    
    if( secondHand ==  0)  { targetPalette = RainbowColors_p; }
    if( secondHand == 20)  { targetPalette = CRGBPalette16( r,r,y,y, p,p,o,o, g,g,b,b, r,g,b,black); }
    if( secondHand == 40)  { targetPalette = CRGBPalette16( black,w,black,w, r,y,b,p, p,b,o,y, g,r,g,r); }
    //if( secondHand == 60)  { targetPalette = LavaColors_p; }
    if( secondHand == 60)  { targetPalette = CRGBPalette16( black,gray,black,r, r,gray,b,gray, p,w,o,w, g,w,g,w); }
    //if( secondHand == 80)  { targetPalette = CloudColors_p; }
    if( secondHand == 80)  { targetPalette = CRGBPalette16( r,black,p,w, black,y,g,o, r,gray,b,o, black,r,g,r); }
    //if( secondHand == 100)  { targetPalette = PartyColors_p; }
    if( secondHand == 100)  { targetPalette = CRGBPalette16( r,b,y,b, p,o,b,r, g,r,b,r, r,g,b,black); }
    
  }
}

void fadein() {

  random16_set_seed(535);                                                           // The randomizer needs to be re-set each time through the loop in order for the 'random' numbers to be the same each time through.

  for (int i = 0; i<NUM_LEDS; i++) {
    uint8_t fader = sin8(millis()/random8(10,20));                                  // The random number for each 'i' will be the same every time.
    leds[i] = ColorFromPalette(currentPalette, i*20, fader, currentBlending);       // Now, let's run it through the palette lookup.
  }

  random16_set_seed(millis());                                                      // Re-randomizing the random number seed for other routines.

} // fadein()
