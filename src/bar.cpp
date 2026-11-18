#include <BMP180.h>
#include <Wire.h>
#include "bar.h"

BMP180 bmp180;
double ambiente;

Bar Bar::init() {
  Bar out;

  while (!bmp180.begin()) {
    Serial.println("Barometro nao conectado");
  }  
  Dados d;
  d.dados[0] = bmp180.readAltitude();
  d.dados[1] = bmp180.readPressure();

  out.vals = d;

  return out;
}

void Bar::atualizaDados(Bar s) {
  s.vals.dados[0] = bmp180.readAltitude();
  s.vals.dados[1]= bmp180.readPressure();
}

Dados Bar::leDados(Bar s) {
  return s.vals;
}

void init_bar() {
  while (!bmp180.begin()) {
    Serial.println("BAR nao conectado");
  }

  ambiente = bmp180.readPressure();
}



float* leDadosBar() {
  float *dados = (float*)malloc(sizeof(float[3]));
  dados[0] = bmp180.readAltitude();
  dados[1] = bmp180.readPressure();
  return 0;
}
