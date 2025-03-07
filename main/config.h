#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define ENCODER_A 4
#define ENCODER_B 5
#define ENCODER_BUTTON 6
#define START_BUTTON 10
#define STOP_BUTTON 9
#define TAP_TEMPO_BUTTON 8
#define CLOCK_OUTPUT 7
#define LED_PIN 15

#define MIN_BPM 30
#define MAX_BPM 200

extern int bpm;
extern int ppqnIndex;
extern bool isRunning;
extern const int ppqnOptions[];

#endif
