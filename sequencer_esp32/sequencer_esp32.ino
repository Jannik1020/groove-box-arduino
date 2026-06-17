#include <Wire.h>
#include <MCP23017_WE.h>
#include <ESP_I2S.h>
#define MCP_ADDRESS 0x20 // (A2/A1/A0 = LOW)
#define MCP_ADDRESS_2 0x21 // (A2/A1/A0 = LOW)

/* A hardware reset is performed during init(). If you want to save a pin you can define a dummy 
 * pin >= 99 and connect the reset pin to HIGH. This will trigger a software reset instead of a 
 * hardware reset. 
 */
#define RESET_PIN 100
#define SDA_PIN 5
#define SCL_PIN 18

#define TMP_ROT_PIN_CLK 23 
#define TMP_ROT_PIN_DT 19

typedef MCP23017_WE MCP; // just for less typing!

/* There are several ways to create your MCP23017 object:
 * MCP23017_WE myMCP = MCP23017_WE(MCP_ADDRESS) -> uses Wire / no reset pin 
 * MCP23017_WE myMCP = MCP23017_WE(MCP_ADDRESS, RESET_PIN) -> uses Wire / RESET_PIN
 * MCP23017_WE myMCP = MCP23017_WE(&Wire, MCP_ADDRESS) -> passing a TwoWire object / no reset pin
 * MCP23017_WE myMCP = MCP23017_WE(&Wire, MCP_ADDRESS, RESET_PIN) -> "all together"
 */
MCP myMCP;
MCP mcp2;
// MCP23017_WE myMCP = MCP23017_WE(MCP_ADDRESS, RESET_PIN); // long version

#include <Keypad_MC17.h>
#include <Keypad.h>        // GDY120705

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {12, 13, 14, 15},
  { 8,  9, 10, 11},
  { 4,  5,  6,  7},
  { 0,  1,  2,  3}
};
byte rowPins[ROWS] = {0,1,2,3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad_MC17 customKeypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS, MCP_ADDRESS_2); 


// 1. Audio-Modus auf I2S setzen
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_I2S_DAC

// 2. Eigene I2S-Pins definieren (Wichtig: Vor den Includes!)
#define MOZZI_I2S_PIN_BCK  25
#define MOZZI_I2S_PIN_DATA 33
#define MOZZI_I2S_PIN_WS   32


#include <Mozzi.h>
#include <Sample.h> // Sample template
#include "../d_kit.h"
Sample <BD_NUM_CELLS, AUDIO_RATE> aBD(BD_DATA);
Sample <SD_NUM_CELLS, AUDIO_RATE> aSD(SD_DATA);
Sample <CH_NUM_CELLS, AUDIO_RATE> aCH(CH_DATA);
Sample <OH_NUM_CELLS, AUDIO_RATE> aOH(OH_DATA);


#include "./ledmatrix.h"
Sequencer sequencer;
LEDMatrix matrix(sequencer);


//rotary encoder
#include "AiEsp32RotaryEncoder.h"
#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 4
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(TMP_ROT_PIN_DT, TMP_ROT_PIN_CLK, 22, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS, false);


int bpmToDelay(int bpm) {
  if(bpm == 0) bpm = 1;
  return (60.0 / bpm) * 1000.0;
}


int tempo = 120;
int tempoMs = bpmToDelay(tempo);

short note = 0;


void setTempo(int newTempo) {
    tempo = newTempo;
    rotaryEncoder.setEncoderValue(newTempo);
    tempoMs = bpmToDelay(newTempo);
}

void IRAM_ATTR readEncoderISR()
{
	rotaryEncoder.readEncoder_ISR();
}



#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <mozzi_midi.h>
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);

#define CONTROL_RATE 64
#include <ADSR.h>
ADSR <CONTROL_RATE, AUDIO_RATE> envelope;

void setup(){ 
  Serial.begin(115200);
  delay(2000);
  Serial.println("trash");
  Wire.begin(SDA_PIN, SCL_PIN);

  myMCP = MCP(&Wire, MCP_ADDRESS, RESET_PIN); // short version
  mcp2 = MCP(&Wire, MCP_ADDRESS_2, 100);

  if(!myMCP.Init()){
    Serial.println("MCP1 Not connected!");
    while(1){} 
  }  
  if(!mcp2.Init()){
    Serial.println("MCP2 Not connected!");
    while(1){} 
  }
  
  customKeypad.begin( );

  myMCP.setPortMode(0b00000000, MCP::A);  // Port A: all pins are INPUT
  myMCP.setPortPullUp(0b11111111, MCP::A); // all internal pullups


  myMCP.setPortMode(0b00000000, MCP::B);  // Port A: all pins are INPUT
  myMCP.setPortPullUp(0b11111111, MCP::B); // all internal pullups

  mcp2.setPortMode(0b00000000, MCP::A);  // Port A: all pins are INPUT
  mcp2.setPortPullUp(0b11111111, MCP::A); // all internal pullups

  mcp2.setPortMode(0b00000000, MCP::B);  // Port A: all pins are INPUT
  mcp2.setPortPullUp(0b11111111, MCP::B); // all internal pullups

  // sequencer.activeInstrument = 0;
  // sequencer.setNote(Note(1,0,1), 0);
  // sequencer.setNote(Note(1,0,1), 4);
  // sequencer.setNote(Note(1,0,1), 8);
  // sequencer.setNote(Note(1,0,1), 12);

  // sequencer.activeInstrument = 1;
  // sequencer.setNote(Note(1,0,1), 2);
  // sequencer.setNote(Note(1,0,1), 6);
  // sequencer.setNote(Note(1,0,1), 10);
  // sequencer.setNote(Note(1,0,1), 14);

  // sequencer.activeInstrument = 2;
  // for(int i = 0; i < 16; i++) {
  //   sequencer.setNote(Note(1,0,1), i);
  // }

  sequencer.activeInstrument = 0;


  startMozzi(CONTROL_RATE);
  aBD.setFreq((float) D_SAMPLERATE / (float) BD_NUM_CELLS);
  aSD.setFreq((float) D_SAMPLERATE / (float) SD_NUM_CELLS);
  aCH.setFreq((float) D_SAMPLERATE / (float) CH_NUM_CELLS);
  aOH.setFreq((float) D_SAMPLERATE / (float) OH_NUM_CELLS);

  aSin.setFreq(440);
  envelope.setADLevels(255, 0);
  envelope.setTimes(10, 80, 0, 200);

  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setAcceleration(0);
  //rotaryEncoder.setEncoderValue(tempo);
  setTempo(tempo);
}


unsigned long lastPolledButtons = 0;
unsigned long lastStepAdvance = 0;
unsigned long lastRendered = 0;
unsigned long currentMillis= 0;
uint16_t status;
uint16_t status2;


void handleSerialInstrumentInput() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    // ignore line endings/spaces from Serial Monitor
    if (c == '\n' || c == '\r' || c == ' ' || c == '\t') continue;

    if (c >= '0' && c <= '2') {
      sequencer.activeInstrument = c - '0';
      Serial.print("activeInstrument=");
      Serial.println(sequencer.activeInstrument);
    } else {
      Serial.println("Use only: 0, 1, or 2");
    }
  }
}

void handleRotaryEncoder()
{
    if (rotaryEncoder.encoderChanged())
	{
		tempo = rotaryEncoder.readEncoder();
        tempoMs = bpmToDelay(tempo);
        Serial.print("Tempo: ");
        Serial.println(tempo);
	}
}


void doubleDivision() {
    setTempo(tempo*2);
}

void halveDivision() {
    setTempo(tempo*2);
}

int envGain = 0;

void updateControl(){
  handleRotaryEncoder();
  handleSerialInstrumentInput();
  currentMillis = millis();

  /* step advance freq */
  if(currentMillis - lastStepAdvance >= tempoMs) {
    sequencer.advanceBeat();
    lastStepAdvance  = currentMillis;

    if(sequencer.noteMatrix[0][sequencer.currentBeat].active) {
        aBD.start();
    }
    if(sequencer.noteMatrix[1][sequencer.currentBeat].active) {
        aSD.start();
    }
    if(sequencer.noteMatrix[2][sequencer.currentBeat].active) {
        aCH.start();
    }
    if(sequencer.noteMatrix[3][sequencer.currentBeat].active) {
        aOH.start();
    }
    if(sequencer.noteMatrix[4][sequencer.currentBeat].active) {
      int midiNote = (4 + 1) * 12 + sequencer.noteMatrix[4][sequencer.currentBeat].pitch;
      aSin.setFreq(mtof(midiNote));
      envelope.noteOn();
      //envelope.noteOff();
    }

  }

  if(currentMillis - lastRendered >= 100) {
    matrix.clear();
    matrix.show();
    lastRendered = currentMillis;
  }

  /* button poll freq */
  if(currentMillis - lastPolledButtons >= 5) {
    uint16_t newStatus = word(myMCP.getPort(MCP::A), myMCP.getPort(MCP::B));
    if(newStatus != status) {
      status = newStatus;

      if(status != word(0b11111111, 0b11111111)) {
        Serial.print ("Buttons  : ");
        Serial.println(status, BIN);

        for (int i = 0; i < 16; i++) {
            if (((status >> i) & 1) == 0) {
              sequencer.toggleNote(note, i);

            }
        }

        for(int i = 0; i < 16; i++) {
            Serial.print(sequencer.noteMatrix[3][i].active);
            Serial.print(" ");
        }
        Serial.println();
        for(int i = 0; i < 16; i++) {
            Serial.print(sequencer.noteMatrix[2][i].active);
            Serial.print(" ");
        }
        Serial.println();
        for(int i = 0; i < 16; i++) {
            Serial.print(sequencer.noteMatrix[1][i].active);
            Serial.print(" ");
        }
        Serial.println();
        for(int i = 0; i < 16; i++) {
            Serial.print(sequencer.noteMatrix[0][i].active);
            Serial.print(" ");
        }
        Serial.println();
        for(int i = 0; i < 16; i++) {
            Serial.print(sequencer.noteMatrix[4][i].active);
            Serial.print(" ");
        }
        Serial.println();

        // for (int i = 0; i < 4; i++) {
        //     if (((status >> i) & 1) == 0) {
        //       switch(i) {
        //         case 0: {
        //           aBD.start();
        //           break;
        //         };
        //         case 1: {
        //           aSD.start();
        //           break;
        //         };
        //         case 2: {
        //           aCH.start();
        //           break;
        //         };
        //         case 3: {
        //           aOH.start();
        //           break;
        //         };
        //       }
        //     }
        // }
      }

    }

    uint16_t newStatus2 = word(mcp2.getPort(MCP::A), mcp2.getPort(MCP::B));
    if(newStatus2 != status2) {
      status2 = newStatus2;
      if(status2 != word(0b11111111, 0b11111111)) {
        Serial.print ("Buttons  : ");
        Serial.println(status2, BIN);

        for (int i = 0; i < 16; i++) {
            if (((status2 >> i) & 1) == 0) {
              sequencer.activeInstrument = i;
            }
        }

        Serial.print ("Instrument: ");
        Serial.println(sequencer.activeInstrument);
        

      }
    }

    short customKey = customKeypad.getKey();
    if (customKey != NO_KEY)
    {
      Serial.println(customKey);

      if(customKey < 16 && customKey > 3) {
        //aSin.setFreq(mtof(midiNote));
        note = customKey - 4;
        int midiNote = (4 + 1) * 12 + note;
        aSin.setFreq(mtof(midiNote));
        envelope.noteOn();

        //envelope.noteOn();
      }
    }
    // else
    // {
    //   envelope.noteOff();
    // }

    envelope.noteOff();
    lastPolledButtons  = currentMillis;
  }

}

AudioOutput updateAudio(){
  // Oszillator-Signal mit der Lautstärke (gain) multiplizieren
  // Durch den Bitshift (>> 8) wird die Lautstärkeskalierung wieder ausgeglichen
  int sinSignal = (aSin.next() * envGain) >> 8;

  int16_t d_sample = aBD.next() + aSD.next() + aCH.next() + aOH.next() + sinSignal;

  envGain = envelope.next();

  return MonoOutput::fromNBit(10, d_sample);
}


void loop() {
  audioHook();
}