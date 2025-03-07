#include "display.h"
#include "config.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Settings
#define OLED_RESET    2   // Some displays require a reset pin
#define SCREEN_ADDRESS 0x3C  // Common I2C address

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

// Ensure Global Variables are Declared
extern int bpm;
extern int ppqnIndex;
extern const int ppqnOptions[];

void initializeDisplay() {

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    delay(1000);
    display.setCursor(0, 0);
    display.print("Stalking your mom..");
    delay(1000);
    display.setCursor(0, 45);
    display.print("Press start..");
    display.display();
    delay(2000);
    updateDisplay();
}

void updateDisplay() {
    display.clearDisplay();
    
    // BPM Title
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.print("BPM");

    // BPM Value
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.print(bpm);

    // PPQN Title
    display.setCursor(100, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.print("PPQN");

    // PPQN Values
    display.setTextSize(1);
    display.setCursor(100, 10);
     for (int i = 0; i < 3; i++) {
        display.setCursor(90, display.getCursorY()); // Align selection indicator
        if (i == ppqnIndex) {
            display.print("> "); // Highlight selected option
        } else {
            display.print("  "); // Maintain alignment
        }
        display.print(ppqnOptions[i]);
        display.setCursor(100, display.getCursorY() + 8);
    }

    display.display();
}
