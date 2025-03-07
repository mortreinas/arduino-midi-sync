#include "taptempo.h"
#include "config.h"
#include "display.h"

unsigned long lastTapTime = 0;
unsigned long tapTimes[3] = {0};

void handleTapTempo() {
    if (digitalRead(TAP_TEMPO_BUTTON) == LOW) {
        unsigned long now = millis();
        if (now - lastTapTime > 2000) {
            for (int i = 0; i < 3; i++) tapTimes[i] = 0;
        }
        tapTimes[2] = tapTimes[1];
        tapTimes[1] = tapTimes[0];
        tapTimes[0] = now;
        if (tapTimes[2] > 0) {
            unsigned long interval = ((tapTimes[0] - tapTimes[1]) + (tapTimes[1] - tapTimes[2])) / 2;
            bpm = constrain(60000 / interval, MIN_BPM, MAX_BPM);
            updateDisplay();
        }
        lastTapTime = now;
    }
}
