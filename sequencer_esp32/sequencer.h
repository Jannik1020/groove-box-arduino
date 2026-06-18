class Note {
  public: 
    unsigned int active: 1;
    unsigned int volume: 4;
    unsigned int pitch : 4;

    Note(): active(0), volume(0), pitch(0) {}
    Note(unsigned int active): active(active), volume(0), pitch(0) {}
    Note(unsigned int active, unsigned int volume, unsigned int pitch): active(active), volume(volume), pitch(pitch) {}
};

class Sequencer {
  public:
  short currentBeat;
  Note noteMatrix[16][16];

  short activeInstrument;

  void setNote(Note note, short beat);
  void toggleNote(int note, short beat);
  void advanceBeat();
  void fromStart();
  void clearRow(int row);
  void pause();
};
