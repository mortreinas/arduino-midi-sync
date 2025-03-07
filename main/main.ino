#include "config.h"
#include "display.h"
#include "midi.h"
#include "taptempo.h"
#include "input.h"
#include "encoder.h"
#include "buttons.h"


int bpm = 120;
int ppqnIndex = 0;
bool isRunning = false;
const int ppqnOptions[] = {24, 4, 2};


void setup() {
    Serial.begin(115200);
    initializeDisplay();
    initializeInputs();
    initializeMidi();
}

void loop() {
  Serial.println("Test");
    handleEncoder();
    handlePPQNSelection();
    handleStartStopButtons();
    handleTapTempo();
    sendMidiClock();
}
