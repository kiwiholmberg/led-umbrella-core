#include <FastLED.h>
#include <Button.h>        //https://github.com/JChristensen/Button

// LED related
#define LED_PIN     2
#define NUM_COLUMNS 8
#define NUM_ROWS    18
#define NUM_LEDS    NUM_COLUMNS * NUM_ROWS
#define BRIGHTNESS  20
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUMBER_OF_PALETTES 3
#define NUMBER_OF_ANIMATIONS 3

// Button related
#define BUTTON_PIN  5
#define BUTTON_PULLUP false
#define BUTTON_INVERT true
#define BUTTON_DEBOUNCE 20

CRGB leds[NUM_LEDS];
Button theButton(BUTTON_PIN, BUTTON_PULLUP, BUTTON_INVERT, BUTTON_DEBOUNCE);

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
int currentAnimationIndex = 0;

CRGBPalette16 availablePalettes[NUMBER_OF_PALETTES] = {
  PartyColors_p, OceanColors_p, RainbowStripeColors_p
};


extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(BRIGHTNESS);

    pinMode(A0, INPUT);

    currentPalette = RainbowStripeColors_p;
    currentBlending = NOBLEND;
}

void loop() {
  static uint8_t startIndex = 0;

  // Motion speed
  startIndex = startIndex + 1;

  theButton.read();

  if (theButton.wasReleased()) {
    if (currentAnimationIndex < NUMBER_OF_ANIMATIONS - 1) {
      currentAnimationIndex = currentAnimationIndex + 1;
    } else {
      currentAnimationIndex = 0;
    }
  }

  switch (currentAnimationIndex) {
    case 0:
      topToBottomScrollAnimation(startIndex);
      break;
    case 1:
      circularScrollAnimation(startIndex);
      break;
    case 2:
      runningClusterAnimation(startIndex);
      break;
  }

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void topToBottomScrollAnimation( uint8_t colorIndex) {
    currentPalette = RainbowStripeColors_p;
    uint8_t brightness = 255;

    for ( int row_index = 0; row_index < NUM_ROWS; row_index++) {  // 0...17
      for( int column_index = 0; column_index < NUM_COLUMNS; column_index++) { // 0...7
        leds[matrix[row_index][column_index]] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
      }
      colorIndex += 3;
    }
}

void circularScrollAnimation( uint8_t colorIndex) {
    currentPalette = RainbowStripeColors_p;
    uint8_t brightness = 255;

    for( int column_index = 0; column_index < NUM_COLUMNS; column_index++) { // 0...7
      for ( int row_index = 0; row_index < NUM_ROWS; row_index++) {  // 0...17
        leds[matrix[row_index][column_index]] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
      }
      colorIndex += 3;
    }
}

void runningClusterAnimation (uint8_t colorIndex) {
  uint8_t brightness = 255;
  static uint8_t currentClusterCol = 0;
  static uint8_t currentClusterRow = 0;
  currentPalette = RainbowColors_p;
  static uint8_t skipCount = 0;

  for( int column_index = 0; column_index < NUM_COLUMNS; column_index++) {
    for ( int row_index = 0; row_index < NUM_ROWS; row_index++) {

      if (column_index == currentClusterCol && row_index == currentClusterRow) {
        // Main col
        leds[matrix[row_index][column_index]] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        // Next col
        if (column_index >= NUM_COLUMNS) {  // On last column
          leds[matrix[row_index][0]] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        } else {
          leds[matrix[row_index][column_index + 1]] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        }
        // Prev col
        if (column_index == 0) { // On first column
          leds[matrix[row_index][NUM_COLUMNS - 2]] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        } else {
          leds[matrix[row_index][column_index - 1]] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        }
      } else {
        leds[matrix[row_index][column_index]] = CRGB::Black;
      }
    }
  }

  // Regulate traversing speed
  if (skipCount < 5) {
    skipCount = skipCount + 1;
    return;
  } else {
    skipCount = 0;
  }

  if (currentClusterRow >= NUM_ROWS - 1) {
    currentClusterRow = 0;
  } else if (currentClusterCol >= NUM_COLUMNS - 1) {
    // On last column, switch to next row.
    currentClusterRow = currentClusterRow + 1;
  }

  if (currentClusterCol >= NUM_COLUMNS - 1) {
    // Last column
    currentClusterCol = 0;
  } else {
    currentClusterCol = currentClusterCol + 1;
  }
}
