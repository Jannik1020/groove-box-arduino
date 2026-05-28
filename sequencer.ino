#include <FastLED.h>
#include "pitches.h"
#define NUM_LEDS 16
#define DATA_PIN 13

//CRGB leds[NUM_LEDS];


class Note {
  public: 
    unsigned int active: 1;
    unsigned int volume: 4;
    unsigned int pitch : 3;

    Note(): active(0), volume(0), pitch(0) {}
    Note(unsigned int active): active(active), volume(0), pitch(0) {}
    Note(unsigned int active, unsigned int volume, unsigned int pitch): active(active), volume(volume), pitch(pitch) {}
};

class Sequencer {
  public:
  short currentBeat;
  Note noteMatrix[16][16];

  short activeInstrument;

  void setNote(Note note, short beat) {
    noteMatrix[activeInstrument][beat] = note;
  }

  void advanceBeat() {
    currentBeat = (currentBeat + 1) % 16;
  }
};


class SoundOutput {
  public:
  Sequencer& sequencer;

  SoundOutput(Sequencer& sequencer): sequencer(sequencer) {

  }

  void play() {
    Note note = sequencer.noteMatrix[0][sequencer.currentBeat];
    if(note.active) {
      tone(10, 440, 200);
    }
  }
};

class LEDMatrix {
  public:
  Sequencer& sequencer;
  CRGB leds[256];

  LEDMatrix(Sequencer& sequencer): sequencer(sequencer) {

    FastLED.addLeds<WS2812, DATA_PIN>(leds, 256);
    FastLED.setBrightness(2);
  }

  void renderRow(int rowIndex, Note row[16], CRGB color) {
    for(int col = 0; col < 16; col++) {
      if(row[col].active) {
        leds[rowIndex * 16 + col] = color;
      }
    }
  }

  void renderRowReverse(int rowIndex, Note row[16], CRGB color) {
    for(int col = 0; col < 16; col++)  {
      if(row[col].active) {
        leds[rowIndex * 16 + (15-col)] = color;
      }
    }
  }

  void renderMatrix() {
    for(int row = 0; row < 16; row++) {
      if(row%2 == 0) {
        renderRow(row, this->sequencer.noteMatrix[row], CRGB::Blue);
      } else {
        renderRowReverse(row, this->sequencer.noteMatrix[row], CRGB::Blue);
      }
    }
  }

  void renderCurrentBeat() {
    leds[sequencer.currentBeat] = CRGB::White;
  }

  void show() {
    renderMatrix();
    renderCurrentBeat();
    FastLED.show();
  }

  void clear() {
    FastLED.clear();
  }
};


Sequencer sequencer;
LEDMatrix matrix(sequencer);
SoundOutput soundOut(sequencer);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  for(int row = 0; row < 16; row++) {
    sequencer.activeInstrument = row;
    sequencer.setNote(Note(1, 0, 1), 4);
  }
  sequencer.activeInstrument = 0;
  sequencer.setNote(Note(1,0,1), 0);
  sequencer.setNote(Note(1,0,1), 8);
  sequencer.setNote(Note(1,0,1), 12);
}

int counter = 0;

int bpmToDelay(int bpm) {
  if(bpm == 0) bpm = 1;
  return (60.0 / bpm) * 1000.0;
}



void loop() {
  // put your main code here, to run repeatedly:
  matrix.clear();
  //leds[counter] = CRGB::Red;
  matrix.show();
  soundOut.play();
  
  Serial.println(bpmToDelay(120));
  delay(bpmToDelay(120));

  sequencer.advanceBeat();
/*
  counter++;
  if(counter >= NUM_LEDS) {
    counter = 0;
  }

  */
}





