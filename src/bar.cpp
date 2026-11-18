#include <BMP180.h>
#include <Wire.h>
#include "bar.h"

BMP180 bmp180;
double ambiente;

void Bar::init() {
  while (!unit.begin()) {
    Serial.println("Barometro nao conectado");
  }  
  vals.dados[0] = bmp180.readAltitude();
  vals.dados[1] = bmp180.readPressure();
}

void Bar::atualizaDados() {
  vals.dados[0] = bmp180.readAltitude();
  vals.dados[1]= bmp180.readPressure();
}

Dados Bar::leDados() const{
  return vals;
}
