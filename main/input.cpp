#include "input.h"
#include "config.h"

void initializeInputs() {
    pinMode(ENCODER_A, INPUT_PULLUP);
    pinMode(ENCODER_B, INPUT_PULLUP);
    pinMode(ENCODER_BUTTON, INPUT_PULLUP);
    pinMode(START_BUTTON, INPUT_PULLUP);
    pinMode(STOP_BUTTON, INPUT_PULLUP);
    pinMode(TAP_TEMPO_BUTTON, INPUT_PULLUP);
    pinMode(CLOCK_OUTPUT, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
}
