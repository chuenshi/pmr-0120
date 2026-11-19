#include <BMP180.h>
#include <Wire.h>
#include "bar.h"


void Bar::init() {
  while (!unit.begin()) {
    Serial.println("Barometro nao conectado");
  }  
  vals.dados[0] = unit.readAltitude();
  vals.dados[1] = unit.readPressure();
  vals.dados[3] = unit.readTemperature();
}

void Bar::atualizaDados() {
  vals.dados[0] = unit.readAltitude();
  vals.dados[1] = unit.readPressure();
  vals.dados[2] = unit.readTemperature();
}

Dados Bar::leDados() const{
  return vals;
}
