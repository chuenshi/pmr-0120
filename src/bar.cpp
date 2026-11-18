#include <BMP180.h>
#include <Wire.h>
#include "bar.h"

double ambiente;

void Bar::init() {
  while (!unit.begin()) {
    Serial.println("Barometro nao conectado");
  }  
  vals.dados[0] = unit.readAltitude();
  vals.dados[1] = unit.readPressure();
}

void Bar::atualizaDados() {
  vals.dados[0] = unit.readAltitude();
  vals.dados[1]= unit.readPressure();
}

Dados Bar::leDados() const{
  return vals;
}
