#include "buttons.h"
#include "config.h"
#include "midi.h"

void handleStartStopButtons() {
    if (digitalRead(START_BUTTON) == LOW && !isRunning) {
        startMidiClock();
    }
    if (digitalRead(STOP_BUTTON) == LOW && isRunning) {
        stopMidiClock();
    }
}
