#ifndef IMU_H
#define IMU_H

#include "sensor.h"

class IMU: public Sensor {
  public:
    void init(); // Retorna um novo IMU
    Dados leDados() const; // Le os dados do IMU
    void atualizaDados(); // Modifica os dados de um IMU
  private:
    Dados vals;
};


void init_IMU();

float* leDadosIMU();

#endif
