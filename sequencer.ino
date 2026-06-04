#include <FastLED.h>
#include "pitches.h"
#define NUM_LEDS 16
#define DATA_PIN 13


#include <Wire.h>
#include <MCP23017_WE.h>
#define MCP_ADDRESS 0x20 // (A2/A1/A0 = LOW)
#define MCP_RESET_PIN 5
typedef MCP23017_WE MCP;
MCP myMCP = MCP(MCP_ADDRESS, MCP_RESET_PIN);

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

  void toggleNote(short beat) {
    noteMatrix[activeInstrument][beat].active = !noteMatrix[activeInstrument][beat].active;
  }

  void advanceBeat() {
    currentBeat = (currentBeat + 1) % 16;
  }

  void toStart() {
    currentBeat = 0;
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

const uint16_t t1_load = 0;
const uint16_t t1_comp = 31250;

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

  
  Wire.begin();
  if(!myMCP.Init()){
    Serial.println("Not connected!");
    while(1){} 
  }  
  myMCP.setPortMode(0b00000000, MCP::A);  // Port A: all pins are INPUT
  myMCP.setPortPullUp(0b11111111, MCP::A); // all internal pullups


  myMCP.setPortMode(0b00000000, MCP::B);  // Port A: all pins are INPUT
  myMCP.setPortPullUp(0b11111111, MCP::B); // all internal pullups

  TCCR1A = 0;

  TCCR1B |= (1 << CS12);
  TCCR1B &= ~(1 << CS11);
  TCCR1B &= ~(1 << CS10);

  TCNT1 = t1_load;
  OCR1A = t1_comp;

  TIMSK1 = (1 << OCIE1A);

  sei();

}

int counter = 0;

int bpmToDelay(int bpm) {
  if(bpm == 0) bpm = 1;
  return (60.0 / bpm) * 1000.0;
}

unsigned long lastStepAdvance = 0;
unsigned long lastRendered = 0;
unsigned long lastPolledButtons = 0;
unsigned long currentMillis= 0;

int tempo = bpmToDelay(120);

byte portStatusA;
byte portStatusB;

uint16_t status;

void play () {
  if(TIMSK1 == 0) {
    TIMSK1 = (1 << OCIE1A);
  } else {
    TIMSK1 = 0;
  }
}

void loop() {
  currentMillis = millis();

  /* render freq */
  if(currentMillis - lastRendered >= 100) {  
    matrix.clear();
    matrix.show();
    lastRendered = currentMillis;

    // Serial.print(portStatusB, BIN);
    // Serial.println(portStatusA, BIN);
  }
  //soundOut.play();
  
  //Serial.println(bpmToDelay(120));
  //delay(bpmToDelay(120));

  // /* step advance freq */
  // if(currentMillis - lastStepAdvance >= tempo) {
  //   sequencer.advanceBeat();
  //   lastStepAdvance  = currentMillis;
  // }

  /* button poll freq */
  if(currentMillis - lastPolledButtons >= 5) {
    uint16_t newStatus = word(myMCP.getPort(MCP::A), myMCP.getPort(MCP::B));
    if(newStatus != status) {
      status = newStatus;

      if(status != word(0b11111111, 0b11111111)) {
        Serial.print ("Buttons: ");
        Serial.println(status, BIN);

        play();

        for (int i = 0; i < 16; i++) {
            if (((status >> i) & 1) == 0) {
              sequencer.toggleNote(i);
            }
        }
      }

    }
    lastPolledButtons  = currentMillis;
  }
}

ISR(TIMER1_COMPA_vect) {
  TCNT1 = t1_load;
  /* step advance freq */
  if(currentMillis - lastStepAdvance >= tempo) {
    sequencer.advanceBeat();
    lastStepAdvance  = currentMillis;
  }
}





