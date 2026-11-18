#ifndef BAR_H
#define BAR_H

#include "sensor.h"


class Bar: public Sensor {
  public:
    Bar init(); // Retorna um novo Bar
    Dados leDados(Bar s); // Le os dados do Bar
    void atualizaDados(Bar s); // Modifica os dados de um Bar
  private:
    Dados vals;
};


void init_bar();

float* leDadosBar();

#endif
