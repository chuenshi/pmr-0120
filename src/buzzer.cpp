#include <Arduino.h>
#include "buzzer.h"

const int PINO_BUZZER = 4;

void Buzzer::init () {
  pinMode (PINO_BUZZER, OUTPUT);
  digitalWrite (PINO_BUZZER, LOW);
}

void Buzzer::ligar() {
  tone(PINO_BUZZER, 1000);
}

void Buzzer::desligar() {
  noTone(PINO_BUZZER);
}
