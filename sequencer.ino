#include <FastLED.h>
#define NUM_LEDS 32
#define DATA_PIN 13

CRGB leds[NUM_LEDS];

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(10);
  Serial.begin(9600);
}

int counter = 0;

int bpmToDelay(int bpm) {
  if(bpm == 0) bpm = 1;
  return (60.0 / bpm) * 1000.0;
}

void loop() {
  // put your main code here, to run repeatedly:
  FastLED.clear();
  leds[counter] = CRGB::Red;
  FastLED.show();
  
  Serial.println(bpmToDelay(120));
  delay(bpmToDelay(120));

  counter++;
  if(counter >= NUM_LEDS) {
    counter = 0;
  }
}
