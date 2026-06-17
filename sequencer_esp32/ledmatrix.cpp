#include "ledmatrix.h"

LEDMatrix::LEDMatrix(Sequencer &sequencer) : sequencer(sequencer)
{

  FastLED.addLeds<WS2812, DATA_PIN>(leds, 256);
  FastLED.setBrightness(this->defaultBrightness);
}

void LEDMatrix::renderRow(int rowIndex, Note row[16], CRGB color)
{
  for (int col = 0; col < 16; col++)
  {
    if (row[col].active)
    {
      leds[rowIndex * 16 + col] = color;
    }
  }
}

void LEDMatrix::renderRowReverse(int rowIndex, Note row[16], CRGB color)
{
  for (int col = 0; col < 16; col++)
  {
    if (row[col].active)
    {
      leds[rowIndex * 16 + (15 - col)] = color;
    }
  }
}

void LEDMatrix::renderMatrix()
{
  for (int row = 0; row < 16; row++)
  {

    CRGB color;
    if(row < rowColors.size()) {
      color = rowColors[row];
    } else {
      color = CRGB::LightGreen;
    }
    if(sequencer.activeInstrument == row) {
      fill_solid(leds + row * 16, 16, color);
      fadeLightBy(leds + row *16, 16, 230);
    }
    if (row % 2 == 0)
    {
      renderRow(row, this->sequencer.noteMatrix[row], color);
    }
    else
    {
      renderRowReverse(row, this->sequencer.noteMatrix[row], color);
    }
  }
}

short mapBeatToLED(short currentBeat, short row) {
  if (row % 2 == 0)
    {
      return 16 * (row) + currentBeat;
    }
    else
    {
      return 16 * (row) + (15-currentBeat);
    }
}

void LEDMatrix::renderCurrentBeat()
{
  leds[mapBeatToLED(this->sequencer.currentBeat, this->sequencer.activeInstrument)] = CRGB::White;
}

void LEDMatrix::show()
{
  renderMatrix();
  renderCurrentBeat();
  FastLED.show();
}

void LEDMatrix::clear()
{
  FastLED.clear();
}