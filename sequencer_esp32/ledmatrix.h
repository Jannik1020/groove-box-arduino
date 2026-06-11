#define FASTLED_ESP32_HAS_RMT 1
#define FASTLED_RMT_MAX_CHANNELS 1

#include <FastLED.h>
#include "sequencer.h"


#define DATA_PIN 27

class LEDMatrix {
  public:
  Sequencer& sequencer;
  CRGB leds[256];

  LEDMatrix(Sequencer& sequencer);

  void renderRow(int rowIndex, Note row[16], CRGB color);
  void renderRowReverse(int rowIndex, Note row[16], CRGB color);
  void renderMatrix();
  void renderCurrentBeat();
  void show();
  void clear();
};