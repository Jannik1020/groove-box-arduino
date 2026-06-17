// filepath: /home/jannikthiele/Studium/microcontroller/groove-box-arduino/sequencer/sequencer.cpp

#include "sequencer.h"

void Sequencer::setNote(Note note, short beat) {
    noteMatrix[activeInstrument][beat] = note;
}

void Sequencer::toggleNote(int note, short beat) {
    noteMatrix[activeInstrument][beat].active = !noteMatrix[activeInstrument][beat].active;
    noteMatrix[activeInstrument][beat].pitch = note;
}

void Sequencer::advanceBeat() {
    currentBeat = (currentBeat + 1) % 16;
}

void Sequencer::fromStart() {
    currentBeat = 0;
}