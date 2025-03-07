#ifndef MIDI_H
#define MIDI_H

#include <Arduino.h>
#include <MIDI.h>
#include <MIDIUSB.h>

void initializeMidi();
void sendMidiClock();
void startMidiClock();
void stopMidiClock();

#endif
