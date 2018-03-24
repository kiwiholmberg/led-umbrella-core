#include <FastLED.h>

#define LED_PIN     2
#define BUTTON_PIN  5
#define NUM_COLUMNS 8
#define NUM_ROWS    18
#define NUM_LEDS    NUM_COLUMNS * NUM_ROWS
#define BRIGHTNESS  20
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUMBER_OF_PALETTES 3

CRGB leds[NUM_LEDS];

// Led numbers arranged in a matrix. Notice first column has reversed order, and all
// other columns increment downwards.
int matrix[NUM_ROWS][NUM_COLUMNS] = {
  {17,18,36,54,72,90,108,126},
  {16,19,37,55,73,91,109,127},
  {15,20,38,56,74,92,110,128},
  {14,21,39,57,75,93,111,129},
  {13,22,40,58,76,94,112,130},
  {12,23,41,59,77,95,113,131},
  {11,24,42,60,78,96,114,132},
  {10,25,43,61,79,97,115,133},
  {9,26,44,62,80,98,116,134},
  {8,27,45,63,81,99,117,135},
  {7,28,46,64,82,100,118,136},
  {6,29,47,65,83,101,119,137},
  {5,30,48,66,84,102,120,138},
  {4,31,49,67,85,103,121,139},
  {3,32,50,68,86,104,122,140},
  {2,33,51,69,87,105,123,141},
  {1,34,52,70,88,106,124,142},
  {0,35,53,71,89,107,125,143}
};


#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType    currentBlending;
int currentPaletteIndex = 0;
int buttonReadVal = HIGH;

CRGBPalette16 availablePalettes[NUMBER_OF_PALETTES] = {
  PartyColors_p, OceanColors_p, RainbowStripeColors_p
};


extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );

    pinMode(A0, INPUT);

    currentPalette = PartyColors_p;//OceanColors_p;//RainbowStripeColors_p;
    currentBlending = NOBLEND;
}

void loop() {
  static uint8_t startIndex = 0;
  // startIndex = startIndex + 1; /* motion speed */
  startIndex = startIndex + 1;

  // TODO: Debounce this button :D 
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (currentPaletteIndex < NUMBER_OF_PALETTES - 1) {
      currentPaletteIndex = currentPaletteIndex + 1;
    } else {
      currentPaletteIndex = 0;
    }
    currentPalette = availablePalettes[currentPaletteIndex];
  }
  
  FillLEDsFromPaletteColors(startIndex);
  
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    

    for ( int row_index = 0; row_index < NUM_ROWS; row_index++) {  // 0...17
      for( int column_index = 0; column_index < NUM_COLUMNS; column_index++) { // 0...7
        leds[matrix[row_index][column_index]] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending); 
      }         
      colorIndex += 3;  // TODO: Make local to row?
    }
        
    
    
}
