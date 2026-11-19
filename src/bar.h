#ifndef BAR_H
#define BAR_H

#include <BMP180.h>
#include <Wire.h>
#include "sensor.h"


class Bar: public Sensor {
  public:
    void init(); // Retorna um novo Bar
    Dados leDados() const; // Le os dados do Bar
    void atualizaDados(); // Modifica os dados de um Bar
  private:
    Dados vals;
    BMP180 unit;
};


#endif
