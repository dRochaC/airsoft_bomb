#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PASS_SIZE 6
#define RIGHT_PASS "------"
#define COUNTER 20

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

char pass[PASS_SIZE] = "";
long counter = -1;
long lastMillis = 0;
long timerResult = 0;

boolean isRunning = true;

void setup() {
  Serial.begin(9600);
  lcd.begin (16, 2);

  pinMode(7, OUTPUT);

  lastMillis = 99999;

  resetPassword();

  Serial.println(checkPassword());
}

void loop() {
  if (millis() - lastMillis > 1000 && isRunning) {
    counter++;

    lcd.clear();
    
    updatePasswordScreen();
    updateTimer();

    if (timerResult < 10 && timerResult >= 0) {
      digitalWrite(7, !digitalRead(7));
    } else {
      digitalWrite(7, LOW);
    }

    if (timerResult == 0) {
      isRunning = false;
    }

    lastMillis = millis();
  }
}

// PASS

boolean checkPassword() {
  for (int i = 0; i < PASS_SIZE; i++) {
    if (RIGHT_PASS[i] != pass[i]) {
      return false;
    }
  }

  return true;
}

void resetPassword() {
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
    lcd.print(pass[i]);
  }
}

void updateTimer() {
  timerResult = COUNTER - counter;
  
  lcd.setCursor(0, 0);
  lcd.print(String(timerResult));
}

