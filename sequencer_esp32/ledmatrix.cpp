#include "ledmatrix.h"

LEDMatrix::LEDMatrix(Sequencer& sequencer): sequencer(sequencer) {

    FastLED.addLeds<WS2812, DATA_PIN>(leds, 256);
    FastLED.setBrightness(2);
  }

  void LEDMatrix::renderRow(int rowIndex, Note row[16], CRGB color) {
    for(int col = 0; col < 16; col++) {
      if(row[col].active) {
        leds[rowIndex * 16 + col] = color;
      }
    }
  }

  void LEDMatrix::renderRowReverse(int rowIndex, Note row[16], CRGB color) {
    for(int col = 0; col < 16; col++)  {
      if(row[col].active) {
        leds[rowIndex * 16 + (15-col)] = color;
      }
    }
  }

  void LEDMatrix::renderMatrix() {
    for(int row = 0; row < 16; row++) {
      if(row%2 == 0) {
        renderRow(row, this->sequencer.noteMatrix[row], CRGB::Blue);
      } else {
        renderRowReverse(row, this->sequencer.noteMatrix[row], CRGB::Blue);
      }
    }
  }

  void LEDMatrix::renderCurrentBeat() {
    leds[sequencer.currentBeat] = CRGB::White;
  }

  void LEDMatrix::show() {
    renderMatrix();
    renderCurrentBeat();
    FastLED.show();
  }

  void LEDMatrix::clear() {
    FastLED.clear();
  }