#include "midi.h"
#include "config.h"

MIDI_CREATE_DEFAULT_INSTANCE();

void initializeMidi() {
    MIDI.begin(MIDI_CHANNEL_OMNI);
}

void sendMidiClock() {
    static unsigned long lastClockTick = 0;
    unsigned long interval = (60000 / bpm) / 24;
    if (isRunning && millis() - lastClockTick >= interval) {
        lastClockTick = millis();
        MIDI.sendRealTime(midi::Clock);
        midiEventPacket_t midiClock = {0x0F, 0xF8, 0x00, 0x00};
        MidiUSB.sendMIDI(midiClock);
        MidiUSB.flush();
    }
}

void startMidiClock() {
    isRunning = true;
    MIDI.sendRealTime(midi::Start);
    midiEventPacket_t startEvent = {0x09, 0xFA, 0x00, 0x00};
    MidiUSB.sendMIDI(startEvent);
    MidiUSB.flush();
}

void stopMidiClock() {
    isRunning = false;
    MIDI.sendRealTime(midi::Stop);
    midiEventPacket_t stopEvent = {0x09, 0xFC, 0x00, 0x00};
    MidiUSB.sendMIDI(stopEvent);
    MidiUSB.flush();
}
