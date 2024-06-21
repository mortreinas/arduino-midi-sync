/*
 * Arduino Midi Master Clock v0.2
 * MIDI master clock/sync/divider for MIDI instruments, Pocket Operators and Korg Volca.
 * by Eunjae Im https://ejlabs.net/arduino-midi-master-clock
 *
 * Required library
 *    TimerOne https://playground.arduino.cc/Code/Timer1
 *    Encoder https://www.pjrc.com/teensy/td_libs_Encoder.html
 *    MIDI https://github.com/FortySevenEffects/arduino_midi_library
 *    Adafruit SSD1306 https://github.com/adafruit/Adafruit_SSD1306
 *******************************************************************************
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the

 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *******************************************************************************
 */

#include <Adafruit_SSD1306.h>
#include <TimerOne.h>
#include <EEPROM.h>
#include <Encoder.h>
#include <MIDI.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define LED_PIN 13 // Tempo LED

#define SYNC_INPUT 12 // Audio Sync in Pin
#define SYNC_OUTPUT_PIN 8 // Audio Sync out Digital Pin 24 ticks
#define SYNC_OUTPUT_PIN2 9 // 2nd Audio Sync out Digital Pin 24 ticks
#define SYNC_OUTPUT_PIN3 10 // 3d Audio Sync out Digital Pin 24 ticks
#define SYNC_OUTPUT_PIN4 11 // 4th Audio Sync out Digital Pin 16 ticks
#define BUTTON_START 7 // StartPush Button
#define BUTTON_STOP 6 // Stop Push Button 
#define BUTTON_TAP_TEMPO 4 //Tap tempo Push Button  (min 3 pushes)
#define BUTTON_ROTARY 5 // Rotary Encoder Button

#define CLOCKS_PER_BEAT 24 // MIDI Clock Ticks
#define AUDIO_SYNC 24 // Audio Sync Ticks
#define AUDIO_SYNC2 42 // 2nd Audio Sync Ticks

#define MINIMUM_BPM 20
#define MAXIMUM_BPM 300
#define LONG_PRESS_THRESHOLD 1000  // 1000 milliseconds for long press


#define BLINK_TIME 4 // LED blink time

// PPQN settings
const int ppqnOptions[] = {2, 24, 48};
int ppqnSetting = 1; // Default to 24PPQN

volatile int  blinkCount = 0,
              AudioSyncCount = 0,
              AudioSyncCount2 = 0;

long intervalMicroSeconds,
      bpm;

boolean playing = false,
      sync_editing = false,
      display_update = false;

Encoder myEnc(2, 3); // Rotary Encoder Pin 2,3 

MIDI_CREATE_DEFAULT_INSTANCE();

//Settings
byte settings = 0;

// Tap tempo variables
#define TAP_COUNT 4
unsigned long tapTimes[TAP_COUNT] = {0};
int tapIndex = 0;
unsigned long lastTapTime = 0;


void setup(void) {
  MIDI.begin(); // MIDI init
  MIDI.turnThruOff();

  bpm = EEPROMReadInt(0);
  if (bpm > MAXIMUM_BPM || bpm < MINIMUM_BPM) {
    bpm = 120;
  }
   
  Timer1.initialize(intervalMicroSeconds);
  Timer1.setPeriod(60L * 1000 * 1000 / bpm / CLOCKS_PER_BEAT);
  Timer1.attachInterrupt(sendClockPulse);  
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);  
  display.setTextSize(4);
  display.setCursor(0,0);
  display.print(bpm);
  display.display();
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_START,INPUT_PULLUP);
  pinMode(BUTTON_STOP,INPUT_PULLUP);
  pinMode(BUTTON_TAP_TEMPO,INPUT_PULLUP);
  pinMode(BUTTON_ROTARY,INPUT_PULLUP);
}

void EEPROMWriteInt(int p_address, int p_value)
     {
     byte lowByte = ((p_value >> 0) & 0xFF);
     byte highByte = ((p_value >> 8) & 0xFF);

     EEPROM.write(p_address, lowByte);
     EEPROM.write(p_address + 1, highByte);
     }

unsigned int EEPROMReadInt(int p_address)
     {
     byte lowByte = EEPROM.read(p_address);
     byte highByte = EEPROM.read(p_address + 1);

     return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

void bpm_display() { 
  updateBpm();
  EEPROMWriteInt(0,bpm);  
  display.setTextSize(4);
  display.setCursor(0,0);  
  display.setTextColor(WHITE, BLACK);
  display.print("     ");
  display.setCursor(0,0);
  display.print(bpm);
  display.display();
  display_update = false;
}

void fuck_display() { 
  EEPROMWriteInt(0,"FUCK");  
  display.setTextSize(4);
  display.setCursor(0,0);  
  display.setTextColor(WHITE, BLACK);
  display.print("     ");
  display.setCursor(0,0);
  display.print("FUCK");
  display.display();
  display_update = false;
}

void displayPPQNSetting() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("D11 PPQN: ");
  display.print(ppqnOptions[ppqnSetting]);
  display.display();
}


void handleStart() {
    if (!playing) {
      playing = true;
      MIDI.sendRealTime(midi::Start);
    } 
}

void handleStop() {
  if (playing) {
    playing = false;
    all_off();
    MIDI.sendRealTime(midi::Stop);
    }
}

int oldPosition;

void loop(void) {
  
  if (digitalRead(BUTTON_START) == LOW) {
    handleStart();
    delay(300); // ugly but just make life easier, no need to check debounce
  }
   if (digitalRead(BUTTON_STOP) == LOW) {
    if(settings == 1) {
      settings = 0;
      bpm_display();
      
    } else {
      handleStop();
    }
    delay(300); // ugly but just make life easier, no need to check debounce
  } 

  if (digitalRead(BUTTON_ROTARY) == LOW) {    
    handleFunctionButtonCombination();
    delay(200);
  }

  handleTapTempo();
  handleEncoder();
}

void handleEncoder() {
   byte i = 0;
  int newPosition = (myEnc.read()/4);
  if (newPosition != oldPosition) {    
    if (oldPosition < newPosition) {
      i = 1;
    } else if (oldPosition > newPosition) {
      i = 2;
    }
    oldPosition = newPosition;
  }
  
  if (!sync_editing) {      
      if (i == 2) {
        bpm++;
        if (bpm > MAXIMUM_BPM) {
          bpm = MAXIMUM_BPM;
        }
        bpm_display();          
      } else if (i == 1) {
        bpm--;
        if (bpm < MINIMUM_BPM) {
          bpm = MINIMUM_BPM;
        }
        bpm_display();
      } else if (settings == 1) {
        //rotary.resetPush();
        displayPPQNSetting();
        sync_editing = true;
      }
  } else  { 
      if (settings == 1) {      
        bpm_display();
        sync_editing = false;
      } else if (i == 1) {      
        
      } else if (i == 2) {
        
      }      
  }
}

void all_off() { // make sure all sync, led pin stat to low
  digitalWrite(SYNC_OUTPUT_PIN, LOW);
  digitalWrite(SYNC_OUTPUT_PIN2, LOW);
  digitalWrite(LED_PIN, LOW);
}

void handleTapTempo() {
  if (digitalRead(BUTTON_TAP_TEMPO) == LOW) {
    unsigned long currentTapTime = millis();
    if (currentTapTime - lastTapTime > 60000 / MINIMUM_BPM) {
      // If the time between taps is too long, reset the tap count
      tapIndex = 0;
    }

    // Record the tap time
    tapTimes[tapIndex] = currentTapTime;
    tapIndex = (tapIndex + 1) % TAP_COUNT;

    // Only calculate BPM if we have at least 3 taps
    if (tapIndex == 0 || tapIndex == 3) {
      unsigned long intervals[TAP_COUNT - 1] = {0};
      for (int i = 0; i < TAP_COUNT - 1; i++) {
        intervals[i] = tapTimes[(tapIndex + i + 1) % TAP_COUNT] - tapTimes[(tapIndex + i) % TAP_COUNT];
      }

      unsigned long averageInterval = 0;
      for (int i = 0; i < TAP_COUNT - 1; i++) {
        averageInterval += intervals[i];
      }
      averageInterval /= (TAP_COUNT - 1);

      unsigned long newBpm = 60000 / averageInterval;
      if (newBpm >= MINIMUM_BPM && newBpm <= MAXIMUM_BPM) {
        bpm = newBpm;
        Timer1.setPeriod(60L * 1000 * 1000 / bpm / CLOCKS_PER_BEAT);
        // Update display or other indicators if needed
        Serial.print("New BPM: ");
        Serial.println(bpm);
      }
    }

    lastTapTime = currentTapTime;
    bpm_display();
    delay(200); // Debounce delay
  }
}

void handleFunctionButtonCombination() {
  displayPPQNSetting();
  settings = 1;
}

void sendClockPulse() {  

  MIDI.sendRealTime(midi::Clock); // sending midi clock
  
  if (playing) {  
  
  blinkCount = (blinkCount + 1) % CLOCKS_PER_BEAT;
  AudioSyncCount = (AudioSyncCount + 1) % AUDIO_SYNC;
  AudioSyncCount2 = (AudioSyncCount2 + 1) % AUDIO_SYNC2;

  if (AudioSyncCount == 0) {
      digitalWrite(SYNC_OUTPUT_PIN, HIGH); 
      digitalWrite(SYNC_OUTPUT_PIN2, HIGH); 
      digitalWrite(SYNC_OUTPUT_PIN3, HIGH); 
      digitalWrite(SYNC_OUTPUT_PIN4, HIGH); 
  } else {        
    if (AudioSyncCount == 1) {     
      digitalWrite(SYNC_OUTPUT_PIN, LOW); 
      digitalWrite(SYNC_OUTPUT_PIN2, LOW); 
      digitalWrite(SYNC_OUTPUT_PIN3, LOW); 
      digitalWrite(SYNC_OUTPUT_PIN4, LOW); 
    }
  }  

  if (AudioSyncCount2 == 0) {
      digitalWrite(SYNC_OUTPUT_PIN4, HIGH);
  } else {        
    if (AudioSyncCount2 == 1) {
      digitalWrite(SYNC_OUTPUT_PIN4, LOW);
    }
  }
  
  if (blinkCount == 0) {
      digitalWrite(LED_PIN, HIGH);      
  } else {
     if (blinkCount == BLINK_TIME) {
       digitalWrite(LED_PIN, LOW);      
     }
  }
  } // if playing
}

void updateBpm() { // update BPM function (on the fly)
  long interval = 60L * 1000 * 1000 / bpm / CLOCKS_PER_BEAT;  
  Timer1.setPeriod(interval);
}
