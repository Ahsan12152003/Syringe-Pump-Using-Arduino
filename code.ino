#include <Keypad.h>
#include <Wire.h>
#include <RTClib.h>
#include <LedControl.h>
#include <Stepper.h>

// Define stepper motor steps per revolution
#define STEPS_PER_REV 200
Stepper stepper(STEPS_PER_REV, 8, 9, 10, 11);

// Keypad configuration
const byte ROWS = 4; 
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 12, 13};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Push buttons
#define START_BUTTON 14
#define STOP_BUTTON 15
#define RESET_BUTTON 16

// RTC module
RTC_DS3231 rtc;

// MAX7219 Display
LedControl lc = LedControl(17, 18, 19, 1);

int flowRate = 0; 
int volume = 0; 
bool running = false;

void setup() {
  Serial.begin(9600);
  stepper.setSpeed(50);
  
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(STOP_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);

  if (!rtc.begin()) {
    Serial.println("RTC not found!");
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    if (key >= '0' && key <= '9') {
      volume = volume * 10 + (key - '0');
    }
    if (key == 'A') {
      flowRate = volume; // Example assignment
      Serial.print("Flow Rate Set: "); Serial.println(flowRate);
    }
  }

  if (digitalRead(START_BUTTON) == LOW) {
    Serial.println("Pump Started");
    running = true;
  }
  if (digitalRead(STOP_BUTTON) == LOW) {
    Serial.println("Pump Stopped");
    running = false;
  }
  if (digitalRead(RESET_BUTTON) == LOW) {
    Serial.println("Reset Parameters");
    volume = 0;
    flowRate = 0;
    running = false;
  }

  if (running) {
    stepper.step(1); // Move plunger forward step-by-step
    displayStatus(volume);
  }
}

void displayStatus(int vol) {
  lc.clearDisplay(0);
  lc.setDigit(0, 3, vol % 10, false);
  lc.setDigit(0, 2, (vol / 10) % 10, false);
}
