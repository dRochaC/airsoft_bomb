#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Keypad.h"

const byte ROWS = 4; // number of rows
const byte COLS = 3; // number of columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'#', '0', '*'}
};

byte rowPins[ROWS] = {8, 7, 6, 5}; // row pinouts of the keypad R1 = D8, R2 = D7, R3 = D6, R4 = D5
byte colPins[COLS] = {4, 3, 2};    // column pinouts of the keypad C1 = D4, C2 = D3, C3 = D2
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

#define PASS_SIZE 6
#define RIGHT_PASS "223344"
#define COUNTER 200
#define BUZZER_PIN 12
#define LED_PIN 9

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

char pass[PASS_SIZE] = "";
long counter = -1;
long lastMillis = 0;
long timerResult = 0;
int passPosition = 0;

boolean isRunning = true;

void setup() {
  Serial.begin(9600);
  lcd.begin (16, 2);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  lastMillis = 99999;

  resetPassword();
}

void loop() {
  char key = keypad.getKey();
  if (key != NO_KEY && passPosition < PASS_SIZE) {
    pass[passPosition] = key;

    if (isFinished()) {
      if (isPasswordCorrect()) {
        finishTimer();
      } else {
        resetPassword();
      }
    } else {
      passPosition++;
    }

    updatePasswordScreen();

    delay(200);
  }

  if (millis() - lastMillis > 1000 && isRunning) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));

    counter++;

    lcd.clear();

    updatePasswordScreen();
    updateTimer();

    if (timerResult < 10 && timerResult >= 0) {
      digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }

    if (timerResult == 0) {
      isRunning = false;
    }

    lastMillis = millis();
  }
}

// PASS

void finishTimer() {
  passPosition++;

  isRunning = false;

  digitalWrite(BUZZER_PIN, HIGH);
}

boolean isFinished() {
  return (passPosition + 1) == PASS_SIZE;
}

boolean isPasswordCorrect() {
  for (int i = 0; i < PASS_SIZE; i++) {
    if (RIGHT_PASS[i] != pass[i]) {
      return false;
    }
  }

  return true;
}

void resetPassword() {
  passPosition = 0;

  for (int i = 0; i < PASS_SIZE; i++) {
    pass[i] = '-';
  }
}

void updatePasswordScreen() {
  lcd.setCursor(0, 1);
  lcd.print("Senha:");

  int offset = 7;
  for (int i = 0; i < PASS_SIZE; i++) {
    lcd.setCursor(i + offset, 1);
    if (passPosition > i) {
      lcd.print("*");
    } else {
      lcd.print("-");
    }
  }
}

void updateTimer() {
  timerResult = COUNTER - counter;

  lcd.setCursor(0, 0);
  lcd.print(String(timerResult));
}

