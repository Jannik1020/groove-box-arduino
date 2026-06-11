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