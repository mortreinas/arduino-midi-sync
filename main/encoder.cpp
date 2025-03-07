#include "encoder.h"
#include "config.h"
#include "display.h"

void handleEncoder() {
    static int lastA = HIGH;
    static int lastB = HIGH;
    static unsigned long lastTurnTime = 0;
    static int stepMultiplier = 1;

    int currentA = digitalRead(ENCODER_A);
    int currentB = digitalRead(ENCODER_B);

    if (currentA != lastA) {  
        unsigned long now = millis();
        unsigned long timeSinceLastStep = now - lastTurnTime;

        if (timeSinceLastStep < 50) { // Faster turns = more BPM changes
            stepMultiplier++;
        } else {
            stepMultiplier = 1;
        }

        if (currentB == LOW) bpm -= stepMultiplier;  // Counterclockwise ➝ Decrease BPM
        else bpm += stepMultiplier;  // Clockwise ➝ Increase BPM

        bpm = constrain(bpm, MIN_BPM, MAX_BPM);
        updateDisplay();
        
        lastTurnTime = now;
    }

    lastA = currentA;
}


void handlePPQNSelection() {
    static unsigned long lastPress = 0;
    
    if (digitalRead(ENCODER_BUTTON) == LOW && millis() - lastPress > 250) { // Debounce
        ppqnIndex = (ppqnIndex + 1) % 3;
        updateDisplay();
        lastPress = millis();
    }
}
