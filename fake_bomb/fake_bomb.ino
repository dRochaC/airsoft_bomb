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

#define DEBUG true
#define PASS_SIZE 6
#define MAX_COUNTER 14400.0 // 4 horas
#define BUZZER_PIN 13
#define LED_PIN 12
#define POT_PIN A0
#define WIRE1_PIN 11
#define WIRE2_PIN A1
#define WIRE3_PIN A2
#define WIRE4_PIN A3

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

char pass[PASS_SIZE] = "";
long selectedCounter = 100000;
long counter = 0;
int counterSpeed = 1;
long lastMillis = 0;
long timerResult = 0;
int passPosition = 0;
String rightPass = ""

                   int wire1Selected = -1;
int wire2Selected = -1;

boolean isRunning = true;

void setup() {
  Serial.begin(9600);

  generateWiresSelected();

  lcd.begin (16, 2);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(WIRE1_PIN, INPUT);
  pinMode(WIRE2_PIN, INPUT);
  pinMode(WIRE3_PIN, INPUT);
  pinMode(WIRE4_PIN, INPUT);

  lastMillis = 99999;

  resetPassword();

  configSystem();
}

void configSystem() {
  lcd.setCursor(0, 1);
  lcd.print("tempo?");

  while (millis() < 30000) {
    float value = analogRead(POT_PIN);
    selectedCounter = (value / 1024) * MAX_COUNTER;

    delay(100);

    updateTimer();
  }

  lcd.setCursor(0, 1);
  lcd.print("senha?");

  int passPos = 0;
  while (millis() < 30000) {
    char key = keypad.getKey();
    if (key != NO_KEY && passPos < PASS_SIZE) {
      lcd.setCursor(7 + passPos, 1);
      lcd.print(key);

      rightPass += key;

      passPos++;

      delay(200);
    }
  }

  Serial.println(rightPass);

  beep(1000);
}

void loop() {
  char key = keypad.getKey();
  if (key != NO_KEY && passPosition < PASS_SIZE) {
    pass[passPosition] = key;

    if (isFinished()) {
      if (isPasswordCorrect()) {
        finishTimer();
      } else {
        beep(1000);

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

    counter += counterSpeed;

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

  checkWires();

  if (!DEBUG) {
    Serial.print(digitalRead(WIRE1_PIN));
    Serial.print(" ");
    Serial.print(digitalRead(WIRE2_PIN));
    Serial.print(" ");
    Serial.print(digitalRead(WIRE3_PIN));
    Serial.print(" ");
    Serial.println(digitalRead(WIRE4_PIN));
  }
}

void beep(int delayTime) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);
  digitalWrite(BUZZER_PIN, LOW);
}

void checkWires() {
  int wrongWire = 0;
  wrongWire += checkWrongWire(WIRE1_PIN, 0);
  wrongWire += checkWrongWire(WIRE2_PIN, 1);
  wrongWire += checkWrongWire(WIRE3_PIN, 2);
  wrongWire += checkWrongWire(WIRE4_PIN, 3);

  if (wrongWire > 0) {

  }
}

int checkWrongWire(int port, int wire) {
  if (digitalRead(port) > 0 && wire1Selected != wire && wire2Selected != wire) {
    return 1;
  }

  return 0;
}

void generateWiresSelected() {
  randomSeed(digitalRead(A5));
  wire1Selected = random(4);
  wire2Selected = wire1Selected;
  while (wire2Selected == wire1Selected) {
    wire2Selected = random(4);
  }

  if (DEBUG) {
    Serial.print(wire1Selected);
    Serial.print(" ");
    Serial.println(wire2Selected);
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
  timerResult = selectedCounter - counter;

  lcd.setCursor(0, 0);
  lcd.print(timeToString(timerResult));
}

char * timeToString(unsigned long t) {
  static char str[12];
  long h = t / 3600;
  t = t % 3600;
  int m = t / 60;
  int s = t % 60;
  sprintf(str, "%04ld:%02d:%02d", h, m, s);
  return str;
}

